varying vec3        fragment_texcoord;
uniform samplerCube env_map_texture;

void main() {
    vec3 flipped_cubemap_texcoord = vec3(fragment_texcoord.x, -fragment_texcoord.y, fragment_texcoord.z);
    gl_FragColor = textureCube(env_map_texture, flipped_cubemap_texcoord);
}