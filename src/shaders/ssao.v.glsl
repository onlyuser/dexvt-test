attribute vec3 vertex_position;
attribute vec3 vertex_normal;
attribute vec2 texcoord;
uniform   mat4 mvp_xform;
uniform   mat4 normal_xform;
varying   vec2 lerp_texcoord;
varying   vec3 lerp_normal;

void main(void) {
    lerp_normal = normalize(vec3(normal_xform*vec4(vertex_normal, 0)));

    gl_Position = mvp_xform*vec4(vertex_position, 1);
    lerp_texcoord = texcoord;
}