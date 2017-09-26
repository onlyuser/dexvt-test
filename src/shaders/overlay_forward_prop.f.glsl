uniform sampler2D color_texture;
uniform sampler2D color_texture2;
varying vec2 lerp_texcoord;

float sigmoid(in float x)
{
    return 1 / (1 + exp(-x));
}

//float sigmoid_dx(float y)
//{
//    return y * (1 - y);
//}

void main(void) {
    vec4 in_vec     = texture2D(color_texture, lerp_texcoord);
    vec4 in_weights = texture2D(color_texture2, lerp_texcoord);

    // draw thin red line
    ivec2 pos = ivec2(gl_FragCoord.xy);
    if(pos.x == 0) {
        gl_FragColor.r = sigmoid(in_vec.r * in_weights.r);
        return;
    }
    gl_FragColor = vec4(0);
}