attribute vec3 coord3d;
attribute vec2 texcoord;
uniform   mat4 mvp_xform;
varying   vec2 f_texcoord;

void main(void) {
    gl_Position = mvp_xform*vec4(coord3d, 1);
    f_texcoord = texcoord;
}