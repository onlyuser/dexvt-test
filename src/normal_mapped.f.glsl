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
    // initialize diffuse/specular lighting
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    // normalize the camera direction
    vec3 camera_direction = normalize(fragment_camera_vector);

    vec2 flipped_texcoord = vec2(fragment_texcoord.x, 1-fragment_texcoord.y);

    vec3 bumpy_surface_normal = normalize(vec3(texture2D(normal_map_texture, flipped_texcoord)));
    vec3 bumpy_world_normal = normalize(fragment_tbn_transform*bumpy_surface_normal);

    // loop through each light
    for(int i = 0; i < NUM_LIGHTS && i < light_count; ++i) {
        if(light_enabled[i] == 0) {
            continue;
        }

        // calculate distance between 0.0 and 1.0
        float dist = min(dot(fragment_light_vector[i], fragment_light_vector[i]), MAX_DIST_SQUARED) / MAX_DIST_SQUARED;
        float distFactor = 1.0 - dist;

        // diffuse
        vec3 light_direction = normalize(fragment_light_vector[i]);
        float diffuseDot = dot(bumpy_world_normal, light_direction);
        diffuse += light_color[i] * clamp(diffuseDot, 0.0, 1.0) * distFactor;

        // specular
        vec3 halfAngle = normalize(camera_direction + light_direction);
        vec3 specularColor = min(light_color[i] + 0.5, 1.0);
        float specularDot = dot(bumpy_world_normal, halfAngle);
        specular += specularColor * pow(clamp(specularDot, 0.0, 1.0), 16.0) * distFactor;
    }

    vec4 sample = texture2D(color_texture, flipped_texcoord); //vec4(1.0, 1.0, 1.0, 1.0);
    gl_FragColor = vec4(clamp(sample.rgb * (diffuse + AMBIENT) + specular, 0.0, 1.0), sample.a);
}
