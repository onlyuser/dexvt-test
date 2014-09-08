const int   BLOOM_KERNEL_SIZE = 5;
const float GLOW_CUTOFF_THRESHOLD = 0.75;

varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
varying vec2      lerp_pixel_offset;
uniform float     bloom_kernel[BLOOM_KERNEL_SIZE];

void sum_components(in vec4 color, out float sum) {
    sum = color.r + color.g + color.b;
}

void main(void) {
    vec4 sum_color;
    for(int i = 0; i < BLOOM_KERNEL_SIZE; i++) {
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
        if(hblur_sum < GLOW_CUTOFF_THRESHOLD) {
            hblur_sample_color = vec4(0);
        }
        if(vblur_sum < GLOW_CUTOFF_THRESHOLD) {
            vblur_sample_color = vec4(0);
        }
        sum_color += (hblur_sample_color + vblur_sample_color)*sample_weight;
    }
    gl_FragColor = clamp(sum_color*0.5, 0, 1);
}