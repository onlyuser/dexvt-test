varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
varying vec2      lerp_pixel_offset;

void main(void) {
    vec2 up     = vec2(lerp_texcoord.x, lerp_texcoord.y + lerp_pixel_offset.y);
    vec2 up2    = vec2(lerp_texcoord.x, lerp_texcoord.y + lerp_pixel_offset.y*2);
    vec2 down   = vec2(lerp_texcoord.x, lerp_texcoord.y - lerp_pixel_offset.y);
    vec2 down2  = vec2(lerp_texcoord.x, lerp_texcoord.y - lerp_pixel_offset.y*2);
    vec2 left   = vec2(lerp_texcoord.x - lerp_pixel_offset.x,   lerp_texcoord.y);
    vec2 left2  = vec2(lerp_texcoord.x - lerp_pixel_offset.x*2, lerp_texcoord.y);
    vec2 right  = vec2(lerp_texcoord.x + lerp_pixel_offset.x,   lerp_texcoord.y);
    vec2 right2 = vec2(lerp_texcoord.x + lerp_pixel_offset.x*2, lerp_texcoord.y);
    gl_FragColor =
            (texture2D(color_texture, lerp_texcoord)*6 +
             texture2D(color_texture, up)*4 +
             texture2D(color_texture, up2) +
             texture2D(color_texture, down)*4 +
             texture2D(color_texture, down2) +
             texture2D(color_texture, left)*4 +
             texture2D(color_texture, left2) +
             texture2D(color_texture, right)*4 +
             texture2D(color_texture, right2))/(6+(1+4)*4);
}