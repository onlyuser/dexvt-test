attribute vec3 vertex_position;
attribute vec3 vertex_normal;
attribute vec3 vertex_tangent;
attribute vec2 texcoord;
uniform   mat4 mvp_xform;
uniform   mat4 model_xform;
uniform   mat4 normal_xform;
varying   vec2 lerp_texcoord;

uniform vec3 camera_position;

varying mat3 lerp_tbn_xform;
varying vec3 lerp_vertex_position_world;
varying vec3 lerp_camera_vector;

const int NUM_LIGHTS = 8;
uniform int light_count;

uniform int light_enabled[NUM_LIGHTS];
uniform vec3 light_position[NUM_LIGHTS];
varying vec3 lerp_light_vector[NUM_LIGHTS];

void main(void) {
    vec3 normal = normalize(vec3(normal_xform*vec4(vertex_normal, 0)));
    vec3 tangent = normalize(vec3(normal_xform*vec4(vertex_tangent, 0)));
    vec3 bitangent = normalize(cross(normal, tangent));
    lerp_tbn_xform = mat3(tangent, bitangent, normal);

    lerp_vertex_position_world = vec3(model_xform*vec4(vertex_position, 1));
    lerp_camera_vector = camera_position - lerp_vertex_position_world;

    for(int i = 0; i < NUM_LIGHTS && i < light_count; i++) {
        if(light_enabled[i] == 0) {
            continue;
        }
        lerp_light_vector[i] = light_position[i] - lerp_vertex_position_world;
    }

    gl_Position = mvp_xform*vec4(vertex_position, 1);
    lerp_texcoord = texcoord;
}