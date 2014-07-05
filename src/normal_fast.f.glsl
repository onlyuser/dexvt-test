varying vec3 fragment_normal;

void main(void) {
    gl_FragColor = vec4((fragment_normal-vec3(-1))*0.5, 0);
}
