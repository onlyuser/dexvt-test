varying   vec3 fragmentNormal;
attribute vec3 coord3d;
attribute vec3 norm3d;
uniform   mat4 mvp_xform;
uniform   mat4 modelview_xform;
uniform   mat4 normal_xform;
uniform   vec3 cameraPosition;
varying   vec3 cameraVector;

void main(void) {
    fragmentNormal = normalize(vec3(normal_xform*vec4(norm3d, 0)));

    vec3 coord3d_world = vec3(modelview_xform*vec4(coord3d, 1));
    cameraVector = cameraPosition - coord3d_world;

    gl_Position = mvp_xform*vec4(coord3d, 1);
}
