varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
varying vec2      lerp_pixel_offset;
uniform float     bloom_kernel[5];

void main(void) {
    vec4 sum_color;
    for(int i = 0; i<5; i++) {
        float sample_weight = bloom_kernel[i];
        ivec2 hblur_kernel_offset = ivec2(i - 2, 0);
        ivec2 vblur_kernel_offset = ivec2(0, i - 2);
        vec2 hblur_sample_coord = vec2(lerp_texcoord.x + lerp_pixel_offset.x*hblur_kernel_offset.x,
                                       lerp_texcoord.y + lerp_pixel_offset.y*hblur_kernel_offset.y);
        vec2 vblur_sample_coord = vec2(lerp_texcoord.x + lerp_pixel_offset.x*vblur_kernel_offset.x,
                                       lerp_texcoord.y + lerp_pixel_offset.y*vblur_kernel_offset.y);
        vec4 hblur_sample_color = texture2D(color_texture, hblur_sample_coord);
        vec4 vblur_sample_color = texture2D(color_texture, vblur_sample_coord);
        sum_color += (hblur_sample_color + vblur_sample_color)*sample_weight;
    }
    gl_FragColor = clamp(sum_color*0.5, 0, 1);
}