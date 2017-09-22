uniform sampler2D in_texture;
//uniform sampler2D weight_texture;
varying vec2 lerp_texcoord;

float rgb_to_float(in vec4  value,
                   in float min_float,
                   in float max_float)
{
    return min_float + (value.r * 65536 + value.g * 256 + value.b) * (max_float - min_float);
}

vec4 float_to_rgb(in float value,
                  in float min_float,
                  in float max_float)
{
    float x = (value - min_float) / (max_float - min_float);
}

void main(void) {
    // draw thin red line
    ivec2 pos = ivec2(gl_FragCoord.xy);
    if(pos.x == 0) {
        gl_FragColor = vec4(1, 0, 0, 0);
        return;
    }

    gl_FragColor = texture2D(in_texture, lerp_texcoord);
}