varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
uniform sampler2D color_texture2;

void main(void) {
    gl_FragColor = max(texture2D(color_texture, lerp_texcoord), texture2D(color_texture2, lerp_texcoord));
    //gl_FragColor = texture2D(color_texture, lerp_texcoord)*0.001 + texture2D(color_texture2, lerp_texcoord);
}