varying vec2      lerp_texcoord;
uniform sampler2D normal_map_texture;

const float AIR_REFRACTIVE_INDEX = 1.0;
const float WATER_REFRACTIVE_INDEX = 1.333;

const float BUMPINESS_FACTOR = 0.001;
const float FRESNEL_REFLECTANCE_SHARPNESS = 2.0;

uniform float reflect_to_refract_ratio;

varying mat3 lerp_tbn_xform;
varying vec3 lerp_camera_vector;

uniform samplerCube env_map_texture;

uniform sampler2D front_depth_overlay_texture;
uniform sampler2D back_depth_overlay_texture;
uniform sampler2D back_normal_overlay_texture;
uniform vec2      viewport_dim;
uniform float     camera_near;
uniform float     camera_far;

void map_depth_to_actual_depth(in float near, in float far, in float z_b, inout float z_e) {
    float z_n = 2.0 * z_b - 1.0;
    z_e = 2.0 * near * far / (far + near - z_n * (far - near));
}

void main(void) {
    vec3 camera_direction = normalize(lerp_camera_vector);

    vec2 flipped_texcoord = vec2(lerp_texcoord.x, 1-lerp_texcoord.y);

    vec3 bumpy_surface_normal =
            mix(vec3(0, 0, 1), normalize(vec3(texture2D(normal_map_texture, flipped_texcoord))), BUMPINESS_FACTOR);
    vec3 bumpy_world_normal = normalize(lerp_tbn_xform*bumpy_surface_normal);

    vec3 reflected_camera_dir = reflect(-camera_direction, bumpy_world_normal);
    float etaR = AIR_REFRACTIVE_INDEX/WATER_REFRACTIVE_INDEX;
    vec3 refracted_camera_dirR = refract(-camera_direction, bumpy_world_normal, etaR);
    vec3 refracted_camera_dirG = refract(-camera_direction, bumpy_world_normal, etaR+0.008);
    vec3 refracted_camera_dirB = refract(-camera_direction, bumpy_world_normal, etaR+0.016);

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

    float one_minus_dot = 1-clamp(dot(camera_direction, bumpy_world_normal), 0, 1);
    float fresnel_reflectance_attenuation = pow(one_minus_dot, FRESNEL_REFLECTANCE_SHARPNESS);

    vec2 overlay_tex_coord = vec2(gl_FragCoord.x/viewport_dim.x, gl_FragCoord.y/viewport_dim.y);

    float front_depth = texture2D(front_depth_overlay_texture, overlay_tex_coord).x;
    float back_depth  = texture2D(back_depth_overlay_texture, overlay_tex_coord).x;

    float front_depth_actual = 0;
    float back_depth_actual  = 0;

    map_depth_to_actual_depth(camera_near, camera_far, front_depth, front_depth_actual);
    map_depth_to_actual_depth(camera_near, camera_far, back_depth, back_depth_actual);

    float thickness = back_depth_actual - front_depth_actual;

    vec4 front_depth_overlay_color = texture2D(front_depth_overlay_texture, overlay_tex_coord);
    vec4 back_depth_overlay_color  = texture2D(back_depth_overlay_texture, overlay_tex_coord);
    vec4 back_normal_overlay_color = texture2D(back_normal_overlay_texture, overlay_tex_coord);

    if(front_depth_actual >= (camera_far-0.1)) {
        gl_FragColor = vec4(1,1,0,0);
    } else if(front_depth_actual <= (camera_near+0.1)) {
        gl_FragColor = vec4(0,1,1,0);
    } else {
        gl_FragColor = mix(vec4(1,0,0,0), vec4(0,0,1,0), front_depth)*0.001 +
                mix(vec4(1,0,0,0), vec4(0,0,1,0), thickness)*0.001 +
                front_depth_overlay_color*0.001 +
                back_depth_overlay_color*0.001 +
                back_normal_overlay_color*0.001 +
                mix(refracted_color, reflected_color, reflect_to_refract_ratio*fresnel_reflectance_attenuation);
    }
}
