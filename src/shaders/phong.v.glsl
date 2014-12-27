attribute vec3 vertex_position;
attribute vec3 vertex_normal;
uniform   mat4 mvp_xform;
uniform   mat4 model_xform;
uniform   mat4 normal_xform;

const int NUM_LIGHTS = 8;
uniform int light_count;

uniform int light_enabled[NUM_LIGHTS];

uniform vec3 camera_pos;
uniform vec3 light_pos[NUM_LIGHTS];

varying vec3 normal;

varying vec3 lerp_camera_vector;
varying vec3 lerp_light_vector[NUM_LIGHTS];

void main(void) {
    normal = normalize(vec3(normal_xform*vec4(vertex_normal, 0)));

    vec3 vertex_position_world = vec3(model_xform*vec4(vertex_position, 1));
    lerp_camera_vector = camera_pos - vertex_position_world;

    for(int i = 0; i < NUM_LIGHTS && i < light_count; i++) {
        if(light_enabled[i] == 0) {
            continue;
        }
        lerp_light_vector[i] = light_pos[i] - vertex_position_world;
    }

    gl_Position = mvp_xform*vec4(vertex_position, 1);
}