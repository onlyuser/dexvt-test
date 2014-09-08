varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
uniform sampler2D bloom_texture;

void main(void) {
    //gl_FragColor = texture2D(color_texture, lerp_texcoord)*0.001 + texture2D(bloom_texture, lerp_texcoord);
    gl_FragColor = max(texture2D(color_texture, lerp_texcoord), texture2D(bloom_texture, lerp_texcoord));
}