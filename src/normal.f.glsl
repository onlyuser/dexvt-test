varying vec2      lerp_texcoord;
uniform sampler2D normal_map_texture;

const float BUMPINESS_FACTOR = 0.001;

varying mat3 lerp_tbn_xform;

void main(void) {
    vec2 flipped_texcoord = vec2(lerp_texcoord.x, 1-lerp_texcoord.y);

    vec3 normal_bump_surface =
            mix(vec3(0, 0, 1), normalize(vec3(texture2D(normal_map_texture, flipped_texcoord))), BUMPINESS_FACTOR);
    vec3 normal_bump_world = normalize(lerp_tbn_xform*normal_bump_surface);

    gl_FragColor = vec4((normal_bump_world-vec3(-1))*0.5, 0);
}
