attribute vec2 texcoord;
attribute vec3 vertex_normal;
attribute vec3 vertex_position;
attribute vec3 vertex_tangent;
uniform mat4 mvp_transform;
uniform mat4 normal_transform;
varying mat3 lerp_tbn_transform;
varying vec2 lerp_texcoord;

void main(void) {
    vec3 normal = normalize(vec3(normal_transform*vec4(vertex_normal, 0)));
    vec3 tangent = normalize(vec3(normal_transform*vec4(vertex_tangent, 0)));
    vec3 bitangent = normalize(cross(normal, tangent));
    lerp_tbn_transform = mat3(tangent, bitangent, normal);

    gl_Position = mvp_transform*vec4(vertex_position, 1);
    lerp_texcoord = texcoord;
}