varying vec3 normal;

void main(void) {
    gl_FragColor = vec4((normal-vec3(-1))*0.5, 0);
}
