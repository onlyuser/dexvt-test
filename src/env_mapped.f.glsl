varying vec2      f_texcoord;
uniform sampler2D normal_map_texture;

const float AIR_REFRACTIVE_INDEX = 1.0;
const float WATER_REFRACTIVE_INDEX = 1.333;

const float BUMPINESS_FACTOR = 0.001;

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
    vec3 refracted_camera_dir = refract(-cameraDir, bumpy_world_normal, AIR_REFRACTIVE_INDEX/WATER_REFRACTIVE_INDEX);

    vec3 reflected_flipped_cubemap_texcoord = vec3(reflected_camera_dir.x, -reflected_camera_dir.y, reflected_camera_dir.z);
    vec3 refracted_flipped_cubemap_texcoord = vec3(refracted_camera_dir.x, -refracted_camera_dir.y, refracted_camera_dir.z);

    vec4 reflected_color = textureCube(env_map_texture, reflected_flipped_cubemap_texcoord);
    vec4 refracted_color = textureCube(env_map_texture, refracted_flipped_cubemap_texcoord);

    gl_FragColor = mix(refracted_color, reflected_color, reflect_to_refract_ratio); 
}
