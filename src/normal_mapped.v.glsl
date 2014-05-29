attribute vec3 coord3d;
attribute vec3 norm3d;
attribute vec3 tangent3d;
attribute vec2 texcoord;
uniform   mat4 mvp_xform;
uniform   mat4 modelview_xform;
uniform   mat4 normal_xform;
varying   vec2 f_texcoord;

const int NUM_LIGHTS = 8;
uniform int lightCount;

uniform int lightEnabled[NUM_LIGHTS];

uniform vec3 cameraPosition;
uniform vec3 lightPosition[NUM_LIGHTS];

varying mat3 tbn_xform;
varying vec3 cameraVector;
varying vec3 lightVector[NUM_LIGHTS];

void main(void) {
    vec3 normal = normalize(vec3(normal_xform*vec4(norm3d, 0)));
    vec3 tangent = normalize(vec3(normal_xform*vec4(tangent3d, 0)));
    vec3 bitangent = normalize(cross(normal, tangent));
    tbn_xform = mat3(tangent, bitangent, normal);

    vec3 coord3d_world = vec3(modelview_xform*vec4(coord3d, 1));
    cameraVector = cameraPosition - coord3d_world;

    for(int i = 0; i < NUM_LIGHTS; ++i) {
        lightVector[i] = lightPosition[i] - coord3d_world;
    }

    gl_Position = mvp_xform*vec4(coord3d, 1);
    f_texcoord = texcoord;
}
