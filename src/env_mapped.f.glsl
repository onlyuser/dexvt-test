varying vec2      f_texcoord;
uniform sampler2D normal_map_texture;

const float AIR_REFRACTIVE_INDEX = 1.0;
const float WATER_REFRACTIVE_INDEX = 1.333;

const float BUMPINESS_FACTOR = 0.001;
const float FRESNEL_REFLECTANCE_SHARPNESS = 2.0;

uniform float reflect_to_refract_ratio;

varying mat3 tbn_xform;
varying vec3 cameraVector;

uniform samplerCube env_map_texture;

void main(void) {
    // normalize the camera direction
    vec3 cameraDir = normalize(cameraVector);

    vec2 flipped_texcoord = vec2(f_texcoord.x, 1-f_texcoord.y);

    vec3 bumpy_surface_normal =
            mix(vec3(0, 0, 1), normalize(vec3(texture2D(normal_map_texture, flipped_texcoord))), BUMPINESS_FACTOR);
    vec3 bumpy_world_normal = normalize(tbn_xform*bumpy_surface_normal);

    vec3 reflected_camera_dir = reflect(-cameraDir, bumpy_world_normal);
    float etaR = AIR_REFRACTIVE_INDEX/WATER_REFRACTIVE_INDEX;
    vec3 refracted_camera_dirR = refract(-cameraDir, bumpy_world_normal, etaR);
    vec3 refracted_camera_dirG = refract(-cameraDir, bumpy_world_normal, etaR+0.008);
    vec3 refracted_camera_dirB = refract(-cameraDir, bumpy_world_normal, etaR+0.016);

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

    float one_minus_dot = 1-clamp(dot(cameraDir, bumpy_world_normal), 0, 1);
    float fresnel_reflectance_attenuation = pow(one_minus_dot, FRESNEL_REFLECTANCE_SHARPNESS);

    gl_FragColor = mix(refracted_color, reflected_color, reflect_to_refract_ratio*fresnel_reflectance_attenuation); 
}
