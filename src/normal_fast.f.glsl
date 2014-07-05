varying vec3 lerp_normal;

void main(void) {
    gl_FragColor = vec4((lerp_normal-vec3(-1))*0.5, 0);
}
