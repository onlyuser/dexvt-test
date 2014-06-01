const float AIR_REFRACTIVE_INDEX = 1.0;
const float WATER_REFRACTIVE_INDEX = 1.333;

attribute vec3 coord3d;
attribute vec3 norm3d;
uniform   mat4 mvp_xform;
uniform   mat4 modelview_xform;
uniform   mat4 normal_xform;
uniform   vec3 cameraPosition;

varying vec3 reflected_flipped_cubemap_texcoord;
varying vec3 refracted_flipped_cubemap_texcoord;

void main(void) {
    vec3 coord3d_world = vec3(modelview_xform*vec4(coord3d, 1));
    vec3 normal_world = normalize(vec3(normal_xform*vec4(norm3d, 0)));

    // normalize the camera direction
    vec3 cameraDir = normalize(cameraPosition - coord3d_world);

    vec3 reflected_camera_dir = reflect(-cameraDir, normal_world);
    vec3 refracted_camera_dir = refract(-cameraDir, normal_world, AIR_REFRACTIVE_INDEX/WATER_REFRACTIVE_INDEX);

    reflected_flipped_cubemap_texcoord = vec3(reflected_camera_dir.x, -reflected_camera_dir.y, reflected_camera_dir.z);
    refracted_flipped_cubemap_texcoord = vec3(refracted_camera_dir.x, -refracted_camera_dir.y, refracted_camera_dir.z);

    gl_Position = mvp_xform*vec4(coord3d, 1);
}
