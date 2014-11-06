varying vec2      lerp_texcoord;
uniform sampler2D random_texture;

const int NUM_SSAO_SAMPLE_KERNELS = 10;
const float SSAO_SAMPLE_RADIUS = 0.5;

uniform sampler2D frontface_depth_overlay_texture;
uniform vec2      viewport_dim;
uniform float     camera_near;
uniform float     camera_far;
uniform vec3      ssao_sample_kernel_pos[NUM_SSAO_SAMPLE_KERNELS];

const float BUMP_FACTOR = 1;

uniform mat4 view_proj_xform;
uniform mat4 inv_mvp_xform;

varying vec3 lerp_normal;

// http://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer?answertab=votes#tab-top
void map_ndc_depth_to_world_depth(
        in    float z_near, // camera near-clipping plane distance from camera
        in    float z_far,  // camera far-clipping plane distance from camera
        in    float z_b,    // projected depth
        inout float z_e)    // actual depth
{
    float z_n = 2.0*z_b - 1.0; // projected depth scaled from [0,1] to [-1,1]
    z_e = 2.0*z_near*z_far/(z_far + z_near - z_n*(z_far - z_near));
}

void map_range(in float s1, in float e1, in float s2, in float e2, in float x, out float out_x)
{
    out_x = s2 + (x - s1)*(e2 - s2)/(e1 - s1);
}

// http://www.songho.ca/opengl/gl_projectionmatrix.html
void unproject_fragment(
        in    vec3  frag_pos,
        in    mat4  _inv_mvp_xform,
        inout vec3  world_pos)
{
    vec4 normalized_device_coord = vec4(frag_pos.x*2 - 1, frag_pos.y*2 - 1 , frag_pos.z*2 - 1, 1);
    vec4 unprojected_coord = _inv_mvp_xform*normalized_device_coord;

    // http://www.iquilezles.org/blog/?p=1911
    unprojected_coord.xyz /= unprojected_coord.w; // perspective divide

    world_pos = unprojected_coord.xyz;
}

// http://mtnphil.wordpress.com/2013/06/26/know-your-ssao-artifacts/
void gen_basis_xform_from_random_vec(
        in    vec3 normal,
        in    vec3 random_vec,
        inout mat3 tbn_xform)
{
    // Gram–Schmidt process to construct an orthogonal basis.
    vec3 tangent = normalize(random_vec - normal*dot(random_vec, normal)); // ortho-basis x-direction
    vec3 bitangent = cross(normal, tangent);                               // ortho-basis y-direction
    tbn_xform = mat3(tangent, bitangent, normal);
}

void main(void) {
    vec2 flipped_texcoord = vec2(lerp_texcoord.x, 1 - lerp_texcoord.y);

    //float frontface_depth_world = 0;
    vec2 overlay_texcoord = vec2(gl_FragCoord.x/viewport_dim.x, gl_FragCoord.y/viewport_dim.y);
    float frontface_ndc_depth = texture2D(frontface_depth_overlay_texture, overlay_texcoord).x;
    //map_ndc_depth_to_world_depth(camera_near, camera_far, frontface_ndc_depth, frontface_depth_world);
    //if(frontface_depth_world >= (camera_far - 0.1)) {
    //    gl_FragColor = vec4(1,1,0,0);
    //    return;
    //} else if(frontface_depth_world <= (camera_near + 0.1)) {
    //    gl_FragColor = vec4(0,1,1,0);
    //    return;
    //}

    // replace incorrect way of getting fragment world position
    // z-depth is measured in rays parallel to camera, not rays emanating from camera
    //vec3 frontface_frag_position_world = camera_pos - camera_direction*frontface_depth_world;
    vec3 frontface_frag_position_world;
    unproject_fragment(vec3(overlay_texcoord, frontface_ndc_depth), inv_mvp_xform, frontface_frag_position_world);
    //if(frontface_frag_position_world.z < -0.9 && frontface_frag_position_world.z > -1.1 &&
    //        frontface_frag_position_world.x > 0 && frontface_frag_position_world.y > 0)
    //{
    //    float x = 0;
    //    map_range(-1.1, -0.9, 0, 1, frontface_frag_position_world.z, x);
    //    gl_FragColor = vec4(x,0,0,0);
    //    return;
    //}

    mat3 tbn_xform;
    vec3 random_unit_normal =
            normalize(texture2D(random_texture, flipped_texcoord).xyz)*2 - vec3(1); // map from [0,1] to [-1,1];
    gen_basis_xform_from_random_vec(lerp_normal, random_unit_normal, tbn_xform);

    float occlusion = 0.0;
    for(int i = 0; i < NUM_SSAO_SAMPLE_KERNELS; i++) {

        vec3 sample_offset_world = normalize(tbn_xform*normalize(ssao_sample_kernel_pos[i]))*SSAO_SAMPLE_RADIUS;
        vec3 sample_world = frontface_frag_position_world + sample_offset_world;
        //if(sample_world.x > 0) {
        //    gl_FragColor = vec4(0,1,0,0);
        //    return;
        //}

        vec4 sample_offset_projected = view_proj_xform*vec4(sample_world, 1.0);
        sample_offset_projected.xyz /= sample_offset_projected.w;
        sample_offset_projected.xy = sample_offset_projected.xy*0.5 + 0.5; // map from [-1,1] to [0,1]
        //if(sample_offset_projected.x > 0.5 && sample_offset_projected.y > 0.5) {
        //    gl_FragColor = vec4(1,0,0,0);
        //    return;
        //}

        float sample_surface_ndc_depth = texture2D(frontface_depth_overlay_texture, sample_offset_projected.xy).x;
        //if(sample_surface_ndc_depth > 0.5) {
        //    gl_FragColor = vec4(1,0,0,0);
        //    return;
        //}

        float sample_surface_depth_world = 0;
        map_ndc_depth_to_world_depth(camera_near, camera_far, sample_surface_ndc_depth, sample_surface_depth_world);
        if(sample_surface_depth_world < 9) { // <== accurate
            gl_FragColor = vec4(1,0,0,0);
            return;
        }

        float sample_depth_world = 0;
        map_ndc_depth_to_world_depth(camera_near, camera_far, sample_offset_projected.z, sample_depth_world);
        //if(sample_depth_world < 8.5) { // <== inaccurate
        //    gl_FragColor = vec4(0,1,0,0);
        //    return;
        //}

        float range_check = abs(frontface_frag_position_world.z - sample_surface_depth_world) < SSAO_SAMPLE_RADIUS ? 1.0 : 0.0;
        //if(range_check == 1) {
        //    gl_FragColor = vec4(0,0,1,0);
        //    return;
        //}

        float hit = sample_depth_world - sample_surface_depth_world > 0.001 ? 1.0 : -1.0;
        //if(range_check > 0 && hit > 0) {
        //    gl_FragColor = vec4(0,1,0,0);
        //    return;
        //}

        occlusion += hit*range_check;
    }
    occlusion /= NUM_SSAO_SAMPLE_KERNELS;

    gl_FragColor = vec4(occlusion,0,0,0);
}