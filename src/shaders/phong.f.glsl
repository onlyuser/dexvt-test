const int NUM_LIGHTS = 8;
uniform int light_count;

const float MAX_DIST = 20;
const float MAX_DIST_SQUARED = MAX_DIST*MAX_DIST;
const int SPECULAR_SHARPNESS = 16;

uniform vec3 light_color[NUM_LIGHTS];
uniform int light_enabled[NUM_LIGHTS];
uniform vec3 ambient_color;

varying vec3 lerp_camera_vector;
varying vec3 lerp_light_vector[NUM_LIGHTS];

varying vec3 normal;

void main(void) {
    vec3 diffuse_sum = vec3(0.0, 0.0, 0.0);
    vec3 specular_sum = vec3(0.0, 0.0, 0.0);

    vec3 camera_direction = normalize(lerp_camera_vector);

    for(int i = 0; i < NUM_LIGHTS && i < light_count; i++) {
        if(light_enabled[i] == 0) {
            continue;
        }

        float dist = min(dot(lerp_light_vector[i], lerp_light_vector[i]), MAX_DIST_SQUARED)/MAX_DIST_SQUARED;
        float distance_factor = 1.0 - dist;

        vec3 light_direction = normalize(lerp_light_vector[i]);
        float diffuse_per_light = dot(normal, light_direction);
        diffuse_sum += light_color[i]*clamp(diffuse_per_light, 0.0, 1.0)*distance_factor;

        vec3 half_angle = normalize(camera_direction + light_direction);
        vec3 specular_color = min(light_color[i] + 0.5, 1.0);
        float specular_per_light = dot(normal, half_angle);
        specular_sum += specular_color*pow(clamp(specular_per_light, 0.0, 1.0), SPECULAR_SHARPNESS)*distance_factor;
    }

    vec4 sample = vec4(1);
    gl_FragColor = vec4(clamp(sample.rgb*(diffuse_sum + ambient_color) + specular_sum, 0.0, 1.0), sample.a);
}