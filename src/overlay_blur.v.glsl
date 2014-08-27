varying vec2 lerp_texcoord;
uniform vec2 viewport_dim;
varying vec2 pixel_offset;

void main(void) {
    gl_Position = gl_Vertex;
    lerp_texcoord = (vec2(gl_Vertex) + vec2(1))*0.5; // map from [-1,1] to [0,1];
    pixel_offset = vec2(1/(viewport_dim.x*0.1), 1/(viewport_dim.y*0.1));
}