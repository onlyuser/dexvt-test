varying vec2      lerp_texcoord;
uniform sampler2D normal_map_texture;

const float AIR_REFRACTIVE_INDEX = 1.0;
const float WATER_REFRACTIVE_INDEX = 1.333;

const float BUMP_FACTOR = 0.001;
const float FRESNEL_REFLECTANCE_SHARPNESS = 2.0;

const int NUM_NEWTONS_METHOD_ITERS = 1;

const float EPSILON = 0.001;

uniform float reflect_to_refract_ratio;

varying mat3 lerp_tbn_xform;
varying vec3 lerp_vertex_position_world;
varying vec3 lerp_camera_vector;

uniform samplerCube env_map_texture;

uniform sampler2D front_depth_overlay_texture;
uniform sampler2D back_depth_overlay_texture;
uniform sampler2D back_normal_overlay_texture;
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
    float z_n = 2.0 * z_b - 1.0; // projected depth scaled from [0, 1] to [-1, 1]
    z_e = 2.0 * z_near * z_far / (z_far + z_near - z_n * (z_far - z_near));
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
    // d = dot(p0 - I0, n) / dot(I, n)
    float num   = dot((plane_orig - orig), plane_normal);
    float denom = dot(dir, plane_normal);
    if((sign(num) != sign(denom)) || (abs(denom) < EPSILON)) {
        orig_intersection_distance = 0;
        return;
    }
    orig_intersection_distance = num / denom;
}

void main(void) {
    vec3 camera_direction = normalize(lerp_camera_vector);
    //vec3 camera_vector = camera_position - lerp_vertex_position_world;
    //vec3 camera_direction = normalize(camera_vector);

    vec2 flipped_texcoord = vec2(lerp_texcoord.x, 1-lerp_texcoord.y);

    vec3 normal_surface =
            mix(vec3(0, 0, 1), normalize(vec3(texture2D(normal_map_texture, flipped_texcoord))), BUMP_FACTOR);
    vec3 normal = normalize(lerp_tbn_xform*normal_surface);

    vec3 reflected_camera_dir = reflect(-camera_direction, normal);
    float etaR = AIR_REFRACTIVE_INDEX/WATER_REFRACTIVE_INDEX;
    vec3 refracted_camera_dirR = refract(-camera_direction, normal, etaR);
    vec3 refracted_camera_dirG = refract(-camera_direction, normal, etaR+0.008);
    vec3 refracted_camera_dirB = refract(-camera_direction, normal, etaR+0.016);

    vec3 reflected_flipped_cubemap_texcoord = vec3(reflected_camera_dir.x, -reflected_camera_dir.y, reflected_camera_dir.z);
    vec3 refracted_flipped_cubemap_texcoordR = vec3(refracted_camera_dirR.x, -refracted_camera_dirR.y, refracted_camera_dirR.z);
    vec3 refracted_flipped_cubemap_texcoordG = vec3(refracted_camera_dirG.x, -refracted_camera_dirG.y, refracted_camera_dirG.z);
    vec3 refracted_flipped_cubemap_texcoordB = vec3(refracted_camera_dirB.x, -refracted_camera_dirB.y, refracted_camera_dirB.z);

    vec4 reflected_color = textureCube(env_map_texture, reflected_flipped_cubemap_texcoord);

    vec4 refracted_color;
    refracted_color.r = textureCube(env_map_texture, refracted_flipped_cubemap_texcoordR).r;
    refracted_color.g = textureCube(env_map_texture, refracted_flipped_cubemap_texcoordG).g;
    refracted_color.b = textureCube(env_map_texture, refracted_flipped_cubemap_texcoordB).b;
    refracted_color.a = 1;

    float one_minus_dot = 1-clamp(dot(camera_direction, normal), 0, 1);
    float fresnel_reflectance_attenuation = pow(one_minus_dot, FRESNEL_REFLECTANCE_SHARPNESS);

    //
    // BEGIN TEST
    //

    vec2 overlay_texcoord = vec2(gl_FragCoord.x/viewport_dim.x, gl_FragCoord.y/viewport_dim.y);

    float front_depth       = texture2D(front_depth_overlay_texture, overlay_texcoord).x;
    float back_depth        = texture2D(back_depth_overlay_texture, overlay_texcoord).x;
    vec3  back_normal_color = texture2D(back_normal_overlay_texture, overlay_texcoord).xyz;
    vec3  back_normal       = back_normal_color*2 - vec3(1); // map from [0,1] to [-1,1]

    float front_depth_actual = 0;
    float back_depth_actual  = 0;

    map_depth_to_actual_depth(camera_near, camera_far, front_depth, front_depth_actual);
    map_depth_to_actual_depth(camera_near, camera_far, back_depth, back_depth_actual);

    float frag_thickness = back_depth_actual - front_depth_actual;

    vec3 back_frag_position_world = camera_position - camera_direction*back_depth_actual;
    //vec3 ray_plane_isect = lerp_vertex_position_world + refracted_camera_dirR*???;

    // apply newton's method to find backface intersection with refracted ray from camera

    vec3  orig         = lerp_vertex_position_world;
    vec3  dir          = refracted_camera_dirR;//-camera_direction;
    vec3  plane_orig   = back_frag_position_world;
    vec3  plane_normal = back_normal;

    // effect: right half window red
    //if(overlay_texcoord.x > 0.5) {
    //    gl_FragColor = vec4(1,0,0,0);
    //    return;
    //}

    // effect: center big red dot (doesn't link "view_proj_xform")
    //if(dot(normalize(plane_orig - orig), normalize(dir)) >= 0.99) {
    //    gl_FragColor = vec4(1,0,0,0);
    //    return;
    //}

    for(int i = 0; i<NUM_NEWTONS_METHOD_ITERS; i++) {
        float orig_intersection_distance = 0;
        intersect_ray_plane(
                orig,                        // point on ray
                dir,                         // ray direction
                plane_orig,                  // point on plane
                plane_normal,                // plane normal
                orig_intersection_distance); // distance between ray-plane intersection and plane

        // effect: thin green border
        if(abs(orig_intersection_distance) < EPSILON) {
            gl_FragColor = vec4(1,0,1,0);
            return;
        }

        vec3 ray_plane_isect = orig + dir*orig_intersection_distance;

        // effect: center big red dot (with jagged-edge)
        //if(dot(normalize(plane_orig - orig), plane_normal) >= 0.9) {
        //    gl_FragColor = vec4(1,0,0,0);
        //    return;
        //}

        vec2 ray_plane_isect_texcoord = vec2(view_proj_xform*vec4(ray_plane_isect, 1));

        // effect: off-centered big red dot only visible at certain angle (when mesh is off-centered)
        //         also thin red border only visible at certain angle (when mesh is off-centered)
        if(distance(overlay_texcoord, ray_plane_isect_texcoord) < 0.25) {
            gl_FragColor = vec4(1,0,0,0);
            return;
        }

        // effect: gradient from red to green.
        //         red when refracted x is bigger.
        //         green when refracted y is bigger.
        if(distance(overlay_texcoord, ray_plane_isect_texcoord) >= 0.25) {
            gl_FragColor = vec4((ray_plane_isect_texcoord-overlay_texcoord+vec2(1))*0.5,0,0); // map from [-1,1] to [0,1]
            return;
        }

        float new_back_depth               = texture2D(back_depth_overlay_texture, ray_plane_isect_texcoord).x;
        vec3  new_back_frag_position_world = camera_position + normalize(ray_plane_isect-camera_position)*new_back_depth;
        vec3  new_back_normal              = texture2D(back_normal_overlay_texture, ray_plane_isect_texcoord).xyz;

        plane_orig   = new_back_frag_position_world;
        plane_normal = new_back_normal;
    }

    //
    // END TEST
    //

    if(front_depth_actual >= (camera_far-0.1)) {
        gl_FragColor = vec4(1,1,0,0);
    } else if(front_depth_actual <= (camera_near+0.1)) {
        gl_FragColor = vec4(0,1,1,0);
    } else {
        gl_FragColor =
                mix(vec4(1,0,0,0), vec4(0,0,1,0), front_depth)*0.001 +
                mix(vec4(1,0,0,0), vec4(0,0,1,0), back_depth)*0.001 +
                mix(vec4(1,0,0,0), vec4(0,0,1,0), frag_thickness)*0.001 +
                vec4(back_normal_color, 0)*0.001 +
                mix(refracted_color, reflected_color, reflect_to_refract_ratio*fresnel_reflectance_attenuation);
    }
}
