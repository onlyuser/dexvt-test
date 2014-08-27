varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
varying vec2      pixel_offset;

void main(void) {
    vec2 up    = vec2(lerp_texcoord.x, lerp_texcoord.y + pixel_offset.y);
    vec2 down  = vec2(lerp_texcoord.x, lerp_texcoord.y - pixel_offset.y);
    vec2 left  = vec2(lerp_texcoord.x - pixel_offset.x, lerp_texcoord.y);
    vec2 right = vec2(lerp_texcoord.x + pixel_offset.x, lerp_texcoord.y);
    gl_FragColor =
            (texture2D(color_texture, lerp_texcoord) +
             texture2D(color_texture, up) +
             texture2D(color_texture, down) +
             texture2D(color_texture, left) +
             texture2D(color_texture, right))/5;
}