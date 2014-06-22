attribute vec3 coord3d;
attribute vec3 norm3d;
uniform   mat4 mvp_xform;
uniform   mat4 normal_xform;
varying   vec3 normal;

void main(void) {
    normal = normalize(vec3(normal_xform*vec4(norm3d, 0)));

    gl_Position = mvp_xform*vec4(coord3d, 1);
}
