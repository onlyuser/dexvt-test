attribute vec2 texcoord;
attribute vec3 vertex_position;
uniform mat4 mvp_transform;
varying vec2 lerp_texcoord;

void main(void) {
    gl_Position = mvp_transform*vec4(vertex_position, 1);
    lerp_texcoord = texcoord;
}