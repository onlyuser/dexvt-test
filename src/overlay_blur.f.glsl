varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
varying vec2      lerp_pixel_offset;
uniform float     bloom_kernel[5];

void sum_components(in vec4 color, out float sum) {
    sum = color.r + color.g + color.b;
}

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
        float hblur_sum;
        float vblur_sum;
        sum_components(hblur_sample_color, hblur_sum);
        sum_components(vblur_sample_color, vblur_sum);
        if(hblur_sum < 0.75) {
            hblur_sample_color = vec4(0);
        }
        if(vblur_sum < 0.75) {
            vblur_sample_color = vec4(0);
        }
        sum_color += (hblur_sample_color + vblur_sample_color)*sample_weight;
    }
    sum_color = clamp(sum_color*0.5, 0, 1);
    gl_FragColor = max(texture2D(color_texture, lerp_texcoord), sum_color);
}