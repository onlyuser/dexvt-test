varying vec2      fragment_texcoord;
uniform sampler2D color_texture;

void main(void) {
    vec2 flipped_texcoord = vec2(fragment_texcoord.x, 1-fragment_texcoord.y);
    gl_FragColor = texture2D(color_texture, flipped_texcoord);
}