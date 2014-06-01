const float AIR_REFRACTIVE_INDEX = 1.0;
const float WATER_REFRACTIVE_INDEX = 1.333;

varying vec3 fragmentNormal;
varying vec3 cameraVector;
uniform samplerCube env_map_texture;
uniform float reflect_to_refract_ratio;

void main(void) {
    vec3 normal = normalize(fragmentNormal);

    // normalize the camera direction
    vec3 cameraDir = normalize(cameraVector);

    vec3 reflected_camera_dir = reflect(-cameraDir, normal);
    vec3 refracted_camera_dir = refract(-cameraDir, normal, AIR_REFRACTIVE_INDEX/WATER_REFRACTIVE_INDEX);

    vec3 reflected_flipped_cubemap_texcoord = vec3(reflected_camera_dir.x, -reflected_camera_dir.y, reflected_camera_dir.z);
    vec3 refracted_flipped_cubemap_texcoord = vec3(refracted_camera_dir.x, -refracted_camera_dir.y, refracted_camera_dir.z);

    vec4 reflected_color = textureCube(env_map_texture, reflected_flipped_cubemap_texcoord);
    vec4 refracted_color = textureCube(env_map_texture, refracted_flipped_cubemap_texcoord);

    gl_FragColor = mix(refracted_color, reflected_color, reflect_to_refract_ratio); 
}
