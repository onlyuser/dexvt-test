varying vec2      f_texcoord;
uniform sampler2D normal_map_texture;

const float BUMPINESS_FACTOR = 0.001;

varying mat3 tbn_xform;
varying vec3 cameraVector;

void main(void) {
    // normalize the camera direction
    vec3 cameraDir = normalize(cameraVector);

    vec2 flipped_texcoord = vec2(f_texcoord.x, 1-f_texcoord.y);

    vec3 bumpy_surface_normal =
            mix(vec3(0, 0, 1), normalize(vec3(texture2D(normal_map_texture, flipped_texcoord))), BUMPINESS_FACTOR);
    vec3 bumpy_world_normal = normalize(tbn_xform*bumpy_surface_normal);

    gl_FragColor = vec4(bumpy_world_normal, 0); 
}
