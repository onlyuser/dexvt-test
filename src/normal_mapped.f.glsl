varying vec2      fragment_texcoord;
uniform sampler2D color_texture;
uniform sampler2D normal_map_texture;

const int NUM_LIGHTS = 8;
uniform int light_count;

const vec3 AMBIENT = vec3(0.1, 0.1, 0.1);
const float MAX_DIST = 20;
const float MAX_DIST_SQUARED = MAX_DIST * MAX_DIST;

uniform vec3 light_color[NUM_LIGHTS];
uniform int light_enabled[NUM_LIGHTS];

varying mat3 fragment_tbn_transform;
varying vec3 fragment_camera_vector;
varying vec3 fragment_light_vector[NUM_LIGHTS];

void main(void) {
    vec3 diffuse_sum = vec3(0.0, 0.0, 0.0);
    vec3 specular_sum = vec3(0.0, 0.0, 0.0);

    vec3 camera_direction = normalize(fragment_camera_vector);

    vec2 flipped_texcoord = vec2(fragment_texcoord.x, 1-fragment_texcoord.y);

    vec3 bumpy_surface_normal = normalize(vec3(texture2D(normal_map_texture, flipped_texcoord)));
    vec3 bumpy_world_normal = normalize(fragment_tbn_transform*bumpy_surface_normal);

    for(int i = 0; i < NUM_LIGHTS && i < light_count; ++i) {
        if(light_enabled[i] == 0) {
            continue;
        }

        float dist = min(dot(fragment_light_vector[i], fragment_light_vector[i]), MAX_DIST_SQUARED) / MAX_DIST_SQUARED;
        float distFactor = 1.0 - dist;

        vec3 light_direction = normalize(fragment_light_vector[i]);
        float diffuse_per_light = dot(bumpy_world_normal, light_direction);
        diffuse_sum += light_color[i] * clamp(diffuse_per_light, 0.0, 1.0) * distFactor;

        vec3 half_angle = normalize(camera_direction + light_direction);
        vec3 specular_color = min(light_color[i] + 0.5, 1.0);
        float specular_per_light = dot(bumpy_world_normal, half_angle);
        specular_sum += specular_color * pow(clamp(specular_per_light, 0.0, 1.0), 16.0) * distFactor;
    }

    vec4 sample = texture2D(color_texture, flipped_texcoord);
    gl_FragColor = vec4(clamp(sample.rgb * (diffuse_sum + AMBIENT) + specular_sum, 0.0, 1.0), sample.a);
}
