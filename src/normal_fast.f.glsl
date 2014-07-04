varying vec3 normal;

void main(void) {
    gl_FragColor = vec4((normal-vec3(-1))*0.5, 0)*0.001+vec4(1,0,0,0);
}
