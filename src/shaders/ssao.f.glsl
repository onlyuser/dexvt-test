varying vec2      lerp_texcoord;
uniform sampler2D bump_texture;

const int NUM_SSAO_SAMPLE_KERNELS = 10;

uniform sampler2D frontface_depth_overlay_texture;
uniform vec2      viewport_dim;
uniform float     camera_near;
uniform float     camera_far;
uniform vec3      camera_pos;
uniform vec3      ssao_sample_kernel_pos[NUM_SSAO_SAMPLE_KERNELS];

const float BUMP_FACTOR = 1;

varying mat3 lerp_tbn_xform;
varying vec3 lerp_vertex_position_world;
uniform mat4 inv_mvp_xform;

// http://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer?answertab=votes#tab-top
void map_depth_to_actual_depth(
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

void main(void) {
    vec2 flipped_texcoord = vec2(lerp_texcoord.x, 1 - lerp_texcoord.y);

    vec3 normal_surface =
            mix(vec3(0, 0, 1), normalize(vec3(texture2D(bump_texture, flipped_texcoord))), BUMP_FACTOR);
    vec3 normal = normalize(lerp_tbn_xform*normal_surface);

    vec3 camera_direction = normalize(camera_pos - lerp_vertex_position_world);

    float frontface_depth_actual = 0;
    vec2 overlay_texcoord = vec2(gl_FragCoord.x/viewport_dim.x, gl_FragCoord.y/viewport_dim.y);
    float frontface_depth = texture2D(frontface_depth_overlay_texture, overlay_texcoord).x;
    map_depth_to_actual_depth(camera_near, camera_far, frontface_depth, frontface_depth_actual);

    if(frontface_depth_actual >= (camera_far - 0.1)) {
        gl_FragColor = vec4(1,1,0,0);
        return;
    } else if(frontface_depth_actual <= (camera_near + 0.1)) {
        gl_FragColor = vec4(0,1,1,0);
        return;
    }

    // replace incorrect way of getting fragment world position
    // z-depth is measured in rays parallel to camera, not rays emanating from camera
    //vec3 frontface_frag_position_world = camera_pos - camera_direction*frontface_depth_actual;
    vec4 projected_coord = vec4(overlay_texcoord.x*2-1, overlay_texcoord.y*2-1, frontface_depth*2-1, 1);
    vec4 unprojected_coord = inv_mvp_xform*projected_coord;
    unprojected_coord.xyz /= unprojected_coord.w;
    vec3 frontface_frag_position_world = unprojected_coord.xyz;

    if(frontface_frag_position_world.z < -0.9 && frontface_frag_position_world.z > -1.1) {
        float x = 0;
        map_range(-1.1, -0.9, 0, 1, frontface_frag_position_world.z, x);
        gl_FragColor = vec4(x,0,0,0);
        return;
    }

    if(camera_pos.x > 0) {
        gl_FragColor = vec4(0,0,1,0);
        return;
    }

    gl_FragColor =
            vec4(ssao_sample_kernel_pos[0], 0)*0.001 +
            vec4(frontface_frag_position_world, 0)*0.001 +
            vec4((normal + vec3(1))*0.5, 0); // map from [-1,1] to [0,1]
}