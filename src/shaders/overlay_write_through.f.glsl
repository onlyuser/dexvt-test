varying vec2 lerp_texcoord;
uniform sampler2D color_texture;

void main(void) {
    gl_FragColor = texture2D(color_texture, lerp_texcoord);
}