uniform sampler2D in_texture;
//uniform sampler2D weight_texture;
varying vec2 lerp_texcoord;

void main(void) {
    // draw thin red line
    ivec2 pos = ivec2(gl_FragCoord.xy);
    if(pos.x == 0) {
        gl_FragColor = vec4(1, 0, 0, 0);
        return;
    }

    vec4 color = texture2D(in_texture, lerp_texcoord);
    if(color.r > 0.5) {
        gl_FragColor = vec4(1, 0, 0, 0);
        return;
    }

    gl_FragColor = texture2D(in_texture, lerp_texcoord);
}