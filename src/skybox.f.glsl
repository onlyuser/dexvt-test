varying vec3 f_texcoord;
uniform samplerCube env_map_texture;

void main() {
    vec3 flipped_cubemap_texcoord = vec3(f_texcoord.x, -f_texcoord.y, f_texcoord.z);
    gl_FragColor = textureCube(env_map_texture, flipped_cubemap_texcoord);
}