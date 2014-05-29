varying vec2      f_texcoord;
uniform sampler2D mytexture;
uniform sampler2D normal_map_texture;

const int NUM_LIGHTS = 8;
uniform int lightCount;

const vec3 AMBIENT = vec3(0.1, 0.1, 0.1);
const float MAX_DIST = 20;
const float MAX_DIST_SQUARED = MAX_DIST * MAX_DIST;

uniform vec3 lightColor[NUM_LIGHTS];
uniform int lightEnabled[NUM_LIGHTS];

varying mat3 tbn_xform;
varying vec3 cameraVector;
varying vec3 lightVector[NUM_LIGHTS];

void main(void) {
    // initialize diffuse/specular lighting
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    // normalize the camera direction
    vec3 cameraDir = normalize(cameraVector);

    vec2 flipped_texcoord = vec2(f_texcoord.x, 1-f_texcoord.y);

    vec3 bumpy_surface_normal = normalize(vec3(texture2D(normal_map_texture, flipped_texcoord)));
    vec3 bumpy_world_normal = normalize(tbn_xform*bumpy_surface_normal);

    // loop through each light
    for(int i = 0; i < NUM_LIGHTS && i < lightCount; ++i) {
        if(lightEnabled[i] == 0) {
            continue;
        }

        // calculate distance between 0.0 and 1.0
        float dist = min(dot(lightVector[i], lightVector[i]), MAX_DIST_SQUARED) / MAX_DIST_SQUARED;
        float distFactor = 1.0 - dist;

        // diffuse
        vec3 lightDir = normalize(lightVector[i]);
        float diffuseDot = dot(bumpy_world_normal, lightDir);
        diffuse += lightColor[i] * clamp(diffuseDot, 0.0, 1.0) * distFactor;

        // specular
        vec3 halfAngle = normalize(cameraDir + lightDir);
        vec3 specularColor = min(lightColor[i] + 0.5, 1.0);
        float specularDot = dot(bumpy_world_normal, halfAngle);
        specular += specularColor * pow(clamp(specularDot, 0.0, 1.0), 16.0) * distFactor;
    }

    vec4 sample = texture2D(mytexture, flipped_texcoord); //vec4(1.0, 1.0, 1.0, 1.0);
    gl_FragColor = vec4(clamp(sample.rgb * (diffuse + AMBIENT) + specular, 0.0, 1.0), sample.a);
}
