varying vec2      lerp_texcoord;
uniform sampler2D color_texture;
varying vec2      lerp_pixel_offset;
uniform float     bloom_kernel[25];

void main(void) {
    vec4 sum_color;
    for(int y = 0; y<5; y++) {
        for(int x = 0; x<5; x++) {
            ivec2 kernel_offset = ivec2(x - 2, y - 2);
            vec2 sample_coord = vec2(lerp_texcoord.x + lerp_pixel_offset.x*kernel_offset.x,
                                     lerp_texcoord.y + lerp_pixel_offset.y*kernel_offset.y);
            vec4 sample_color = texture2D(color_texture, sample_coord);
            float sample_weight = bloom_kernel[y*5 + x];
            sum_color += clamp(sample_color*sample_weight, 0, 1);
        }
    }
    gl_FragColor = sum_color;
}