varying vec2      lerp_texcoord;
uniform sampler2D normal_map_texture;

const float AIR_REFRACTIVE_INDEX = 1.0;
const float GLASS_REFRACTIVE_INDEX = 1.5;
const float GLASS_REFRACTIVE_INDEX_RGB_OFFSET = 0.01;

const float BUMP_FACTOR = 0.001;
const float FRESNEL_REFLECTANCE_SHARPNESS = 2.0;
const float BEERS_LAW_FALLOFF_SHARPNESS = 1;

const int NUM_NEWTONS_METHOD_ITERS = 3;

const float EPSILON = 0.001;

uniform float reflect_to_refract_ratio;

varying mat3 lerp_tbn_xform;
varying vec3 lerp_vertex_position_world;
varying vec3 lerp_camera_vector;

uniform samplerCube env_map_texture;

uniform sampler2D frontface_depth_overlay_texture;
uniform sampler2D backface_depth_overlay_texture;
uniform sampler2D backface_normal_overlay_texture;
uniform vec2      viewport_dim;
uniform float     camera_near;
uniform float     camera_far;

uniform vec3 camera_position;
uniform mat4 view_proj_xform;

// http://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer?answertab=votes#tab-top
void map_depth_to_actual_depth(
        in    float z_near, // camera near-clipping plane distance from camera
        in    float z_far,  // camera far-clipping plane distance from camera
        in    float z_b,    // projected depth
        inout float z_e)    // actual depth
{
    float z_n = 2.0*z_b - 1.0; // projected depth scaled from [0,1] to [-1,1]
    z_e = 2.0*z_near*z_far/(z_far + z_near - z_n*(z_far - z_near));
}

// http://glm.g-truc.net/0.9.5/api/a00203.html#ga6203e3a0822575ced2b2cd500b396b0c
// http://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection#Algebraic_form
void intersect_ray_plane(
        in    vec3  orig,                       // point on ray   (I0)
        in    vec3  dir,                        // ray direction  (I)
        in    vec3  plane_orig,                 // point on plane (p0)
        in    vec3  plane_normal,               // plane normal   (n)
        inout float orig_intersection_distance) // distance between ray-plane intersection and plane
{
    // d = dot(p0 - I0, n)/dot(I, n)
    float num   = dot(plane_orig - orig, plane_normal);
    float denom = dot(dir, plane_normal);
    if((sign(num) != sign(denom)) || (abs(denom) < EPSILON)) {
        orig_intersection_distance = 0;
        return;
    }
    orig_intersection_distance = num/denom;
}

void sample_env_map(
        in    vec3        ray_direction,
        in    samplerCube env_map_texture,
        inout vec4        env_color)
{
    vec3 flipped_cubemap_texcoord = vec3(ray_direction.x, -ray_direction.y, ray_direction.z);
    env_color = textureCube(env_map_texture, flipped_cubemap_texcoord);
}

void reflect_into_env_map(
        in    vec3        ray_direction,
        in    vec3        surface_normal,
        in    samplerCube env_map_texture,
        inout vec4        reflected_color)
{
    vec3 reflected_camera_dir = reflect(ray_direction, surface_normal);
    sample_env_map(reflected_camera_dir, env_map_texture, reflected_color);
}

void refract_into_env_map(
        in    vec3        ray_direction,
        in    vec3        surface_normal,
        in    float       eta,
        in    samplerCube env_map_texture,
        inout vec4        refracted_color,
        inout vec3        refracted_camera_dir)
{
    refracted_camera_dir = refract(ray_direction, surface_normal, eta);
    sample_env_map(refracted_camera_dir, env_map_texture, refracted_color);
}

// http://en.wikipedia.org/wiki/Chromatic_aberration
// http://en.wikipedia.org/wiki/Dispersion_(optics)
// http://www.opticampus.com/cecourse.php?url=chromatic_aberration
void refract_into_env_map_ex(
        in    vec3        ray_direction,
        in    vec3        surface_normal,
        in    float       eta,
        in    float       eta_rgb_offset,
        in    samplerCube env_map_texture,
        inout vec4        refracted_color,
        inout vec3        refracted_camera_dir)
{
    // wiki: refraction indices of most transparent materials (e.g., air, glasses) decrease with increasing wavelength
    vec3 refracted_camera_dirR = refract(ray_direction, surface_normal, eta - eta_rgb_offset);
    vec3 refracted_camera_dirG = refract(ray_direction, surface_normal, eta);
    vec3 refracted_camera_dirB = refract(ray_direction, surface_normal, eta + eta_rgb_offset);
    vec3 refracted_flipped_cubemap_texcoordR = vec3(refracted_camera_dirR.x, -refracted_camera_dirR.y, refracted_camera_dirR.z);
    vec3 refracted_flipped_cubemap_texcoordG = vec3(refracted_camera_dirG.x, -refracted_camera_dirG.y, refracted_camera_dirG.z);
    vec3 refracted_flipped_cubemap_texcoordB = vec3(refracted_camera_dirB.x, -refracted_camera_dirB.y, refracted_camera_dirB.z);
    refracted_color.r = textureCube(env_map_texture, refracted_flipped_cubemap_texcoordR).r;
    refracted_color.g = textureCube(env_map_texture, refracted_flipped_cubemap_texcoordG).g;
    refracted_color.b = textureCube(env_map_texture, refracted_flipped_cubemap_texcoordB).b;
    refracted_color.a = 1;
    refracted_camera_dir = refracted_camera_dirG;
}

void newtons_method_update(
        in    sampler2D _backface_depth_overlay_texture,
        in    sampler2D _backface_normal_overlay_texture,
        in    vec3      orig,
        in    vec3      dir,
        inout vec3      plane_orig,
        inout vec3      plane_normal)
{
    float orig_intersection_distance = 0;
    intersect_ray_plane(
            orig,                        // point on ray
            dir,                         // ray direction
            plane_orig,                  // point on plane
            plane_normal,                // plane normal
            orig_intersection_distance); // distance between ray-plane intersection and plane

    // 1st chance abort on glancing edge
    //if(abs(orig_intersection_distance) < EPSILON) {
    //    gl_FragColor = vec4(0,1,0,0);//frontface_refracted_color;
    //    return;
    //}

    vec3 ray_plane_isect = orig + normalize(dir)*orig_intersection_distance;

    vec4 ray_plane_isect_texcoord_raw = view_proj_xform*vec4(ray_plane_isect, 1);
    ray_plane_isect_texcoord_raw /= ray_plane_isect_texcoord_raw.w; // perspective divide

    vec2 ray_plane_isect_texcoord = (vec2(ray_plane_isect_texcoord_raw) + vec2(1))*0.5; // map from [-1,1] to [0,1]

    float new_backface_depth = texture2D(_backface_depth_overlay_texture, ray_plane_isect_texcoord).x;

    float new_backface_depth_actual = 0;
    map_depth_to_actual_depth(camera_near, camera_far, new_backface_depth, new_backface_depth_actual);

    vec3 new_backface_frag_position_world = camera_position + normalize(ray_plane_isect-camera_position)*new_backface_depth_actual;
    vec4 new_backface_normal_color        = texture2D(_backface_normal_overlay_texture, ray_plane_isect_texcoord);
    vec3 new_backface_normal              = -normalize(new_backface_normal_color.xyz*2 - vec3(1)); // map from [0,1] to [-1,1]

    plane_orig   = new_backface_frag_position_world;
    plane_normal = new_backface_normal;
}

void main(void) {
    vec3 camera_direction = normalize(lerp_camera_vector);

    // another way to get camera direction
    //vec3 camera_direction = normalize(camera_position - lerp_vertex_position_world);

    vec2 flipped_texcoord = vec2(lerp_texcoord.x, 1 - lerp_texcoord.y);

    vec3 normal_surface =
            mix(vec3(0, 0, 1), normalize(vec3(texture2D(normal_map_texture, flipped_texcoord))), BUMP_FACTOR);
    vec3 normal = normalize(lerp_tbn_xform*normal_surface);

    // frontface reflection component
    vec4 reflected_color;
    reflect_into_env_map(-camera_direction, normal, env_map_texture, reflected_color);

    // frontface refraction component with chromatic dispersion
    float frontface_eta = AIR_REFRACTIVE_INDEX/GLASS_REFRACTIVE_INDEX;
    float frontface_eta_red = AIR_REFRACTIVE_INDEX/(GLASS_REFRACTIVE_INDEX - GLASS_REFRACTIVE_INDEX_RGB_OFFSET);
    float frontface_eta_rgb_offset = abs(frontface_eta - frontface_eta_red);
    vec4 frontface_refracted_color;
    vec3 frontface_refracted_camera_dir;
    refract_into_env_map_ex(
            -camera_direction,
            normal,
            frontface_eta,
            frontface_eta_rgb_offset,
            env_map_texture,
            frontface_refracted_color,
            frontface_refracted_camera_dir);

    vec2 overlay_texcoord = vec2(gl_FragCoord.x/viewport_dim.x, gl_FragCoord.y/viewport_dim.y);

    if(overlay_texcoord.x < 0.5) {
        // frontface fresnel component
        float frontface_fresnel_reflectance =
                pow(1 - clamp(dot(camera_direction, normal), 0, 1), FRESNEL_REFLECTANCE_SHARPNESS);
        gl_FragColor =
                mix(frontface_refracted_color, reflected_color,
                        max(reflect_to_refract_ratio, frontface_fresnel_reflectance));
        return;
    }

    float frontface_depth       = texture2D(frontface_depth_overlay_texture, overlay_texcoord).x;
    float backface_depth        = texture2D(backface_depth_overlay_texture, overlay_texcoord).x;
    vec4  backface_normal_color = texture2D(backface_normal_overlay_texture, overlay_texcoord);
    vec3  backface_normal       = -normalize(backface_normal_color.xyz*2 - vec3(1)); // map from [0,1] to [-1,1]

    float frontface_depth_actual = 0;
    float backface_depth_actual  = 0;

    map_depth_to_actual_depth(camera_near, camera_far, frontface_depth, frontface_depth_actual);
    map_depth_to_actual_depth(camera_near, camera_far, backface_depth, backface_depth_actual);

    float frag_thickness = backface_depth_actual - frontface_depth_actual;

    vec3 backface_frag_position_world = camera_position - camera_direction*backface_depth_actual;
    //vec3 ray_plane_isect = lerp_vertex_position_world + frontface_refracted_camera_dir*???;

    // apply newton's method to find backface intersection with refracted ray from camera

    vec3 orig         = lerp_vertex_position_world;
    vec3 dir          = normalize(frontface_refracted_camera_dir);
    vec3 plane_orig   = backface_frag_position_world;
    vec3 plane_normal = normalize(backface_normal);

    for(int i = 0; i < NUM_NEWTONS_METHOD_ITERS; i++) {
        newtons_method_update(
                backface_depth_overlay_texture,
                backface_normal_overlay_texture,
                orig,
                dir,
                plane_orig,
                plane_normal);
    }

    // backface refraction component with chromatic dispersion
    float backface_eta = GLASS_REFRACTIVE_INDEX/AIR_REFRACTIVE_INDEX;
    float backface_eta_red = (GLASS_REFRACTIVE_INDEX - GLASS_REFRACTIVE_INDEX_RGB_OFFSET)/AIR_REFRACTIVE_INDEX;
    float backface_eta_rgb_offset = abs(backface_eta - backface_eta_red);
    vec4 backface_refracted_color;
    vec3 backface_refracted_camera_dir;
    refract_into_env_map_ex(
            dir,
            plane_normal,
            backface_eta,
            -backface_eta_rgb_offset,
            env_map_texture,
            backface_refracted_color,
            backface_refracted_camera_dir);

    // backface reflection component
    // 2nd chance abort on total internal reflection
    if(distance(backface_refracted_camera_dir, vec3(0)) < EPSILON) {
        vec3 backface_reflected_camera_dir = reflect(frontface_refracted_camera_dir, plane_normal);
        vec4 total_internal_reflected_color;
        sample_env_map(backface_reflected_camera_dir, env_map_texture, total_internal_reflected_color);
        backface_refracted_color = total_internal_reflected_color;
    }

    float beers_law_transmittance = exp(-frag_thickness*BEERS_LAW_FALLOFF_SHARPNESS);

    // backface fresnel component
    float backface_fresnel_reflectance =
            pow(1 - clamp(dot(camera_direction, plane_normal), 0, 1), FRESNEL_REFLECTANCE_SHARPNESS);

    vec4 inside_color =
            mix(frontface_refracted_color, backface_refracted_color,
                    max(beers_law_transmittance, backface_fresnel_reflectance));

    if(frontface_depth_actual >= (camera_far - 0.1)) {
        gl_FragColor = vec4(1,1,0,0);
    } else if(frontface_depth_actual <= (camera_near + 0.1)) {
        gl_FragColor = vec4(0,1,1,0);
    } else {
        // frontface fresnel component
        float frontface_fresnel_reflectance =
                pow(1 - clamp(dot(camera_direction, normal), 0, 1), FRESNEL_REFLECTANCE_SHARPNESS);
        gl_FragColor =
                mix(vec4(1,0,0,0), vec4(0,0,1,0), frontface_depth)*0.001 +
                mix(vec4(1,0,0,0), vec4(0,0,1,0), backface_depth)*0.001 +
                mix(vec4(1,0,0,0), vec4(0,0,1,0), frag_thickness)*0.001 +
                backface_normal_color*0.001 +
                mix(inside_color, reflected_color,
                        max(reflect_to_refract_ratio, frontface_fresnel_reflectance));
    }
}