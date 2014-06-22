attribute vec3 coord3d;
attribute vec3 norm3d;
uniform   mat4 mvp_xform;
varying   vec3 normal;

void main(void) {
    normal = normalize(norm3d);

    gl_Position = mvp_xform*vec4(coord3d, 1);
}
