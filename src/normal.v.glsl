attribute vec3 coord3d;
attribute vec3 norm3d;
attribute vec3 tangent3d;
attribute vec2 texcoord;
uniform   mat4 mvp_xform;
uniform   mat4 normal_xform;
varying   vec2 f_texcoord;

varying mat3 tbn_xform;

void main(void) {
    vec3 normal = normalize(vec3(normal_xform*vec4(norm3d, 0)));
    vec3 tangent = normalize(vec3(normal_xform*vec4(tangent3d, 0)));
    vec3 bitangent = normalize(cross(normal, tangent));
    tbn_xform = mat3(tangent, bitangent, normal);

    gl_Position = mvp_xform*vec4(coord3d, 1);
    f_texcoord = texcoord;
}
