/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/glut.h>
/* GLM */
// #define GLM_MESSAGES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader_utils.h>

#include "res_texture.c"
#include "res_texture2.c"

#include <Buffer.h>
#include <Camera.h>
#include <FrameBuffer.h>
#include <ShaderContext.h>
#include <Light.h>
#include <Material.h>
#include <Mesh.h>
#include <PrimitiveFactory.h>
#include <Program.h>
#include <Shader.h>
#include <Scene.h>
#include <Texture.h>
#include <Util.h>
#include <VarAttribute.h>
#include <VarUniform.h>
#include <iostream> // std::cout
#include <sstream> // std::stringstream
#include <iomanip> // std::setprecision

#define HI_RES_TEX_DIM  512
#define MED_RES_TEX_DIM 256
#define LO_RES_TEX_DIM  128
#define BLUR_ITERS      10

const char* DEFAULT_CAPTION = "My Textured Cube";

int init_screen_width = 800, init_screen_height = 600;
vt::Camera* camera;
vt::Mesh* mesh_skybox, *mesh_overlay, *mesh, *mesh2, *mesh3, *mesh4, *mesh5, *mesh6, *mesh7, *mesh8, *mesh9, *mesh10, *mesh11, *mesh12, *mesh13, *mesh14, *mesh15;
vt::Light* light, *light2, *light3;
std::unique_ptr<vt::FrameBuffer> frontface_depth_overlay_fb, backface_depth_overlay_fb, backface_normal_overlay_fb, hi_res_color_overlay_fb, med_res_color_overlay_fb, lo_res_color_overlay_fb;

bool left_mouse_down = false, right_mouse_down = false;
glm::vec2 prev_mouse_coord, mouse_drag;
glm::vec3 prev_orient, orient, orbit_speed = glm::vec3(0, -0.5, -0.5);
float prev_orbit_radius = 0, orbit_radius = 8, dolly_speed = 0.1, light_distance = 4;
bool wire_frame_mode = false;
bool show_fps = false;
bool show_vert_normals = false;
bool show_lights = false;
bool show_diamond = false;
bool post_process_blur = false;
int overlay_mode = 0;

int texture_index = 0;
int demo_mode = 0;
float prev_zoom = 0, zoom = 1, ortho_dolly_speed = 0.1;

float angle = 0;

vt::Material* overlay_write_through_material;
vt::Material* overlay_blur_material;
vt::Material* overlay_bloom_material;

int init_resources()
{
    vt::Scene *scene = vt::Scene::instance();

    mesh_skybox = vt::PrimitiveFactory::create_viewport_quad("grid");
    scene->set_skybox(mesh_skybox);

    mesh_overlay = vt::PrimitiveFactory::create_viewport_quad("grid3");
    scene->set_overlay(mesh_overlay);

    scene->add_mesh(mesh   = vt::PrimitiveFactory::create_box(        "box"));
    scene->add_mesh(mesh2  = vt::PrimitiveFactory::create_box(        "box2"));
    scene->add_mesh(mesh3  = vt::PrimitiveFactory::create_grid(       "grid",       16, 16,  10, 10));
    scene->add_mesh(mesh4  = vt::PrimitiveFactory::create_sphere(     "sphere",     16, 16,  0.5));
    scene->add_mesh(mesh5  = vt::PrimitiveFactory::create_torus(      "torus",      16, 16,  0.5, 0.25));
    scene->add_mesh(mesh6  = vt::PrimitiveFactory::create_cylinder(   "cylinder",   16, 0.5, 1));
    scene->add_mesh(mesh7  = vt::PrimitiveFactory::create_cone(       "cone",       16, 0.5, 1));
    scene->add_mesh(mesh8  = vt::PrimitiveFactory::create_hemisphere( "hemisphere", 16, 16,  0.5));
    scene->add_mesh(mesh9  = vt::PrimitiveFactory::create_tetrahedron("tetrahedron"));
    scene->add_mesh(mesh10 = vt::PrimitiveFactory::create_diamond_brilliant_cut("diamond"));
    scene->add_mesh(mesh11 = vt::PrimitiveFactory::create_box(        "box3"));
    scene->add_mesh(mesh12 = vt::PrimitiveFactory::create_diamond_brilliant_cut("diamond2"));
    scene->add_mesh(mesh13 = vt::PrimitiveFactory::create_sphere(     "sphere2",    16, 16,  0.5));
    scene->add_mesh(mesh14 = vt::PrimitiveFactory::create_box(        "box4"));
    scene->add_mesh(mesh15 = vt::PrimitiveFactory::create_grid(       "grid2",      32, 32,  1, 1));

    mesh->set_origin(glm::vec3(-0.5, -0.5, -0.5));  // box
    mesh2->set_scale(glm::vec3(0.5, 2, 3));         // box2
    mesh3->set_origin(glm::vec3(-5, 5, -2.5));      // grid
    mesh4->set_origin(glm::vec3(2, 0, 0));          // sphere
    mesh5->set_origin(glm::vec3(-2, 0, 0));         // torus
    mesh6->set_origin(glm::vec3(0, -2.5, 0));       // cylinder
    mesh7->set_origin(glm::vec3(0, 1.5, 0));        // cone
    mesh8->set_origin(glm::vec3(-2, 1.75, 0));      // hemisphere
    mesh9->set_origin(glm::vec3(1.5, 1.5, -0.5));   // tetrahedron
    mesh10->set_origin(glm::vec3(-2, -2.5, 0));     // diamond
    mesh11->set_origin(glm::vec3(1.5, -2.5, -0.5)); // box3
    mesh12->set_origin(glm::vec3(0, -1, 0));        // diamond2
    mesh13->set_origin(glm::vec3(0, 0, 0));         // sphere2
    mesh14->set_origin(glm::vec3(-1, -1, -1));      // box4
    mesh15->set_origin(glm::vec3(-2, 0, -2));       // grid2

    mesh3->set_orient(glm::vec3(0, -90, 0));

    mesh2->set_visible(false);
    mesh3->set_visible(false);
    //mesh6->set_visible(false);
    //mesh7->set_visible(false);
    mesh12->set_visible(false);
    mesh13->set_visible(false);
    mesh14->set_visible(false);
    mesh15->set_visible(false);

    mesh12->set_scale(glm::vec3(2, 2, 2)); // diamond2
    mesh13->set_scale(glm::vec3(2, 2, 2)); // sphere2
    mesh14->set_scale(glm::vec3(2, 2, 2)); // box4
    mesh15->set_scale(glm::vec3(4, 4, 4)); // grid2

    vt::Material* normal_mapped_material = new vt::Material(
            "normal_mapped",
            "src/normal_mapped.v.glsl",
            "src/normal_mapped.f.glsl",
            false,  // use_normal_only
            false,  // use_camera_vec
            true,   // use_phong_shading
            true,   // use_texture_mapping
            true,   // use_normal_mapping
            false,  // use_env_mapping
            false,  // use_depth_overlay
            false,  // use_bloom_kernel
            false,  // skybox
            false); // overlay
    scene->add_material(normal_mapped_material);

    vt::Material* skybox_material = new vt::Material(
            "skybox",
            "src/skybox.v.glsl",
            "src/skybox.f.glsl",
            false,  // use_normal_only
            false,  // use_camera_vec
            false,  // use_phong_shading
            false,  // use_texture_mapping
            false,  // use_normal_mapping
            false,  // use_env_mapping
            false,  // use_depth_overlay
            false,  // use_bloom_kernel
            true,   // skybox
            false); // overlay
    scene->add_material(skybox_material);

    overlay_write_through_material = new vt::Material(
            "overlay_write_through",
            "src/overlay_write_through.v.glsl",
            "src/overlay_write_through.f.glsl",
            false, // use_normal_only
            false, // use_camera_vec
            false, // use_phong_shading
            false, // use_texture_mapping
            false, // use_normal_mapping
            false, // use_env_mapping
            false, // use_depth_overlay
            false, // use_bloom_kernel
            false, // skybox
            true); // overlay
    scene->add_material(overlay_write_through_material);

    overlay_blur_material = new vt::Material(
            "overlay_blur",
            "src/overlay_blur.v.glsl",
            "src/overlay_blur.f.glsl",
            false, // use_normal_only
            false, // use_camera_vec
            false, // use_phong_shading
            false, // use_texture_mapping
            false, // use_normal_mapping
            false, // use_env_mapping
            false, // use_depth_overlay
            true,  // use_bloom_kernel
            false, // skybox
            true); // overlay
    scene->add_material(overlay_blur_material);

    overlay_bloom_material = new vt::Material(
            "overlay_bloom",
            "src/overlay_bloom.v.glsl",
            "src/overlay_bloom.f.glsl",
            false, // use_normal_only
            false, // use_camera_vec
            false, // use_phong_shading
            false, // use_texture_mapping
            false, // use_normal_mapping
            false, // use_env_mapping
            false, // use_depth_overlay
            false, // use_bloom_kernel
            false, // skybox
            true); // overlay
    scene->add_material(overlay_bloom_material);

    vt::Material* texture_mapped_material = new vt::Material(
            "texture_mapped",
            "src/texture_mapped.v.glsl",
            "src/texture_mapped.f.glsl",
            false,  // use_normal_only
            false,  // use_camera_vec
            false,  // use_phong_shading
            true,   // use_texture_mapping
            false,  // use_normal_mapping
            false,  // use_env_mapping
            false,  // use_depth_overlay
            false,  // use_bloom_kernel
            false,  // skybox
            false); // overlay
    scene->add_material(texture_mapped_material);

    vt::Material* env_mapped_material = new vt::Material(
            "env_mapped",
            "src/env_mapped.v.glsl",
            "src/env_mapped.f.glsl",
            false,  // use_normal_only
            false,  // use_camera_vec
            false,  // use_phong_shading
            false,  // use_texture_mapping
            true,   // use_normal_mapping
            true,   // use_env_mapping
            false,  // use_depth_overlay
            false,  // use_bloom_kernel
            false,  // skybox
            false); // overlay
    scene->add_material(env_mapped_material);

    vt::Material* env_mapped_ex_material = new vt::Material(
            "env_mapped_ex",
            "src/env_mapped_ex.v.glsl",
            "src/env_mapped_ex.f.glsl",
            false,  // use_normal_only
            false,  // use_camera_vec
            true,   // use_phong_shading
            false,  // use_texture_mapping
            true,   // use_normal_mapping
            true,   // use_env_mapping
            true,   // use_depth_overlay
            false,  // use_bloom_kernel
            false,  // skybox
            false); // overlay
    scene->add_material(env_mapped_ex_material);

    vt::Material* env_mapped_material_fast = new vt::Material(
            "env_mapped_fast",
            "src/env_mapped_fast.v.glsl",
            "src/env_mapped_fast.f.glsl",
            false,  // use_normal_only
            false,  // use_camera_vec
            false,  // use_phong_shading
            false,  // use_texture_mapping
            false,  // use_normal_mapping
            true,   // use_env_mapping
            false,  // use_depth_overlay
            false,  // use_bloom_kernel
            false,  // skybox
            false); // overlay
    scene->add_material(env_mapped_material_fast);

    vt::Material* normal_material = new vt::Material(
            "normal",
            "src/normal.v.glsl",
            "src/normal.f.glsl",
            true,   // use_normal_only
            false,  // use_camera_vec
            false,  // use_phong_shading
            false,  // use_texture_mapping
            true,   // use_normal_mapping
            false,  // use_env_mapping
            false,  // use_depth_overlay
            false,  // use_bloom_kernel
            false,  // skybox
            false); // overlay
    scene->add_material(normal_material);

    vt::Material* normal_material_fast = new vt::Material(
            "normal_fast",
            "src/normal_fast.v.glsl",
            "src/normal_fast.f.glsl",
            true,   // use_normal_only
            false,  // use_camera_vec
            false,  // use_phong_shading
            false,  // use_texture_mapping
            false,  // use_normal_mapping
            false,  // use_env_mapping
            false,  // use_depth_overlay
            false,  // use_bloom_kernel
            false,  // skybox
            false); // overlay
    scene->add_material(normal_material_fast);
    scene->set_normal_material(normal_material_fast);

    vt::Texture* texture = new vt::Texture(
            "dex3d",
            res_texture.width,
            res_texture.height,
            res_texture.pixel_data);
    scene->add_texture(                  texture);
    normal_mapped_material->add_texture( texture);
    texture_mapped_material->add_texture(texture);

    vt::Texture* texture2 = new vt::Texture(
            "lode_runner",
            res_texture2.width,
            res_texture2.height,
            res_texture2.pixel_data);
    scene->add_texture(                 texture2);
    normal_mapped_material->add_texture(texture2);

    vt::Texture* texture3 = new vt::Texture(
            "chesterfield_color",
            "data/chesterfield_color.png");
    scene->add_texture(                 texture3);
    normal_mapped_material->add_texture(texture3);
    env_mapped_material->add_texture(   texture3);
    env_mapped_ex_material->add_texture(texture3);

    vt::Texture* texture4 = new vt::Texture(
            "chesterfield_normal",
            "data/chesterfield_normal.png");
    scene->add_texture(                 texture4);
    normal_mapped_material->add_texture(texture4);
    env_mapped_material->add_texture(   texture4);
    env_mapped_ex_material->add_texture(texture4);
    normal_material->add_texture(       texture4);

    vt::Texture* texture5 = new vt::Texture(
            "skybox_texture",
            "data/SaintPetersSquare2/posx.png",
            "data/SaintPetersSquare2/negx.png",
            "data/SaintPetersSquare2/posy.png",
            "data/SaintPetersSquare2/negy.png",
            "data/SaintPetersSquare2/posz.png",
            "data/SaintPetersSquare2/negz.png");
    scene->add_texture(                   texture5);
    skybox_material->add_texture(         texture5);
    env_mapped_material->add_texture(     texture5);
    env_mapped_ex_material->add_texture(  texture5);
    env_mapped_material_fast->add_texture(texture5);

    vt::Texture* frontface_depth_overlay_texture = new vt::Texture(
            "frontface_depth_overlay",
            HI_RES_TEX_DIM,
            HI_RES_TEX_DIM,
            NULL,
            vt::Texture::DEPTH);
    texture_mapped_material->add_texture(       frontface_depth_overlay_texture);
    env_mapped_ex_material->add_texture(        frontface_depth_overlay_texture);
    overlay_write_through_material->add_texture(frontface_depth_overlay_texture);
    overlay_blur_material->add_texture(         frontface_depth_overlay_texture);

    vt::Texture* backface_depth_overlay_texture = new vt::Texture(
            "backface_depth_overlay",
            HI_RES_TEX_DIM,
            HI_RES_TEX_DIM,
            NULL,
            vt::Texture::DEPTH);
    texture_mapped_material->add_texture(       backface_depth_overlay_texture);
    env_mapped_ex_material->add_texture(        backface_depth_overlay_texture);
    overlay_write_through_material->add_texture(backface_depth_overlay_texture);
    overlay_blur_material->add_texture(         backface_depth_overlay_texture);

    vt::Texture* backface_normal_overlay_texture = new vt::Texture(
            "backface_normal_overlay",
            HI_RES_TEX_DIM,
            HI_RES_TEX_DIM,
            NULL,
            vt::Texture::RGB);
    texture_mapped_material->add_texture(       backface_normal_overlay_texture);
    env_mapped_ex_material->add_texture(        backface_normal_overlay_texture);
    overlay_write_through_material->add_texture(backface_normal_overlay_texture);
    overlay_blur_material->add_texture(         backface_normal_overlay_texture);

    vt::Texture* hi_res_color_overlay_texture = new vt::Texture(
            "hi_res_color_overlay",
            HI_RES_TEX_DIM,
            HI_RES_TEX_DIM,
            NULL,
            vt::Texture::RGB);
    texture_mapped_material->add_texture(       hi_res_color_overlay_texture);
    overlay_write_through_material->add_texture(hi_res_color_overlay_texture);
    overlay_blur_material->add_texture(         hi_res_color_overlay_texture);
    overlay_bloom_material->add_texture(        hi_res_color_overlay_texture);

    vt::Texture* med_res_color_overlay_texture = new vt::Texture(
            "med_res_color_overlay",
            MED_RES_TEX_DIM,
            MED_RES_TEX_DIM,
            NULL,
            vt::Texture::RGB);
    texture_mapped_material->add_texture(       med_res_color_overlay_texture);
    overlay_write_through_material->add_texture(med_res_color_overlay_texture);
    overlay_blur_material->add_texture(         med_res_color_overlay_texture);

    vt::Texture* lo_res_color_overlay_texture = new vt::Texture(
            "lo_res_color_overlay",
            LO_RES_TEX_DIM,
            LO_RES_TEX_DIM,
            NULL,
            vt::Texture::RGB);
    texture_mapped_material->add_texture(       lo_res_color_overlay_texture);
    overlay_write_through_material->add_texture(lo_res_color_overlay_texture);
    overlay_blur_material->add_texture(         lo_res_color_overlay_texture);
    overlay_bloom_material->add_texture(        lo_res_color_overlay_texture);

    glm::vec3 origin = glm::vec3();
    camera = new vt::Camera("camera", origin+glm::vec3(0, 0, orbit_radius), origin);
    scene->set_camera(camera);

    frontface_depth_overlay_fb = std::unique_ptr<vt::FrameBuffer>(new vt::FrameBuffer(frontface_depth_overlay_texture, camera));
    backface_depth_overlay_fb  = std::unique_ptr<vt::FrameBuffer>(new vt::FrameBuffer(backface_depth_overlay_texture, camera));
    backface_normal_overlay_fb = std::unique_ptr<vt::FrameBuffer>(new vt::FrameBuffer(backface_normal_overlay_texture, camera));
    hi_res_color_overlay_fb    = std::unique_ptr<vt::FrameBuffer>(new vt::FrameBuffer(hi_res_color_overlay_texture, camera));
    med_res_color_overlay_fb   = std::unique_ptr<vt::FrameBuffer>(new vt::FrameBuffer(med_res_color_overlay_texture, camera));
    lo_res_color_overlay_fb    = std::unique_ptr<vt::FrameBuffer>(new vt::FrameBuffer(lo_res_color_overlay_texture, camera));

    scene->add_light(light  = new vt::Light("light1", origin+glm::vec3(light_distance, 0, 0), glm::vec3(1, 0, 0)));
    scene->add_light(light2 = new vt::Light("light2", origin+glm::vec3(0, light_distance, 0), glm::vec3(0, 1, 0)));
    scene->add_light(light3 = new vt::Light("light3", origin+glm::vec3(0, 0, light_distance), glm::vec3(0, 0, 1)));

    mesh_skybox->set_material(skybox_material);
    mesh_skybox->set_texture_index(mesh_skybox->get_material()->get_texture_index_by_name("skybox_texture"));

    mesh_overlay->set_material(overlay_write_through_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("hi_res_color_overlay"));

    // box
    mesh->set_material(normal_mapped_material);
    mesh->set_texture_index(           mesh->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh->set_normal_map_texture_index(mesh->get_material()->get_texture_index_by_name("chesterfield_normal"));

    // box2
    mesh2->set_material(normal_mapped_material);
    mesh2->set_texture_index(           mesh2->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh2->set_normal_map_texture_index(mesh2->get_material()->get_texture_index_by_name("chesterfield_normal"));

    // grid
    mesh3->set_material(texture_mapped_material);
    //mesh3->set_texture_index(mesh3->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    //mesh3->set_texture_index(mesh3->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    //mesh3->set_texture_index(mesh3->get_material()->get_texture_index_by_name("backface_normal_overlay"));
    mesh3->set_texture_index(mesh3->get_material()->get_texture_index_by_name("hi_res_color_overlay"));

    // sphere
    mesh4->set_material(env_mapped_ex_material);
    mesh4->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh4->set_texture_index(                        mesh4->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh4->set_normal_map_texture_index(             mesh4->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh4->set_frontface_depth_overlay_texture_index(mesh4->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh4->set_backface_depth_overlay_texture_index( mesh4->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh4->set_backface_normal_overlay_texture_index(mesh4->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // torus
    mesh5->set_material(env_mapped_material);
    mesh5->set_reflect_to_refract_ratio(1); // 100% reflective
    mesh5->set_texture_index(           mesh5->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh5->set_normal_map_texture_index(mesh5->get_material()->get_texture_index_by_name("chesterfield_normal"));

    // cylinder
    mesh6->set_material(texture_mapped_material);
    mesh6->set_texture_index(mesh6->get_material()->get_texture_index_by_name("dex3d"));

    // cone
    mesh7->set_material(texture_mapped_material);
    mesh7->set_texture_index(mesh7->get_material()->get_texture_index_by_name("dex3d"));

    // hemisphere
    mesh8->set_material(env_mapped_material_fast);

    // tetrahedron
    mesh9->set_material(env_mapped_ex_material);
    mesh9->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh9->set_texture_index(                        mesh9->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh9->set_normal_map_texture_index(             mesh9->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh9->set_frontface_depth_overlay_texture_index(mesh9->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh9->set_backface_depth_overlay_texture_index( mesh9->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh9->set_backface_normal_overlay_texture_index(mesh9->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // diamond
    mesh10->set_material(env_mapped_material_fast);
    mesh10->set_reflect_to_refract_ratio(0.33); // 33% reflective

    // box3
    mesh11->set_material(env_mapped_ex_material);
    mesh11->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh11->set_texture_index(                        mesh11->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh11->set_normal_map_texture_index(             mesh11->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh11->set_frontface_depth_overlay_texture_index(mesh11->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh11->set_backface_depth_overlay_texture_index( mesh11->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh11->set_backface_normal_overlay_texture_index(mesh11->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // diamond2
    mesh12->set_material(env_mapped_ex_material);
    mesh12->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh12->set_texture_index(                        mesh12->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh12->set_normal_map_texture_index(             mesh12->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh12->set_frontface_depth_overlay_texture_index(mesh12->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh12->set_backface_depth_overlay_texture_index( mesh12->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh12->set_backface_normal_overlay_texture_index(mesh12->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // sphere2
    mesh13->set_material(env_mapped_ex_material);
    mesh13->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh13->set_texture_index(                        mesh13->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh13->set_normal_map_texture_index(             mesh13->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh13->set_frontface_depth_overlay_texture_index(mesh13->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh13->set_backface_depth_overlay_texture_index( mesh13->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh13->set_backface_normal_overlay_texture_index(mesh13->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // box4
    mesh14->set_material(env_mapped_ex_material);
    mesh14->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh14->set_texture_index(                        mesh14->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh14->set_normal_map_texture_index(             mesh14->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh14->set_frontface_depth_overlay_texture_index(mesh14->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh14->set_backface_depth_overlay_texture_index( mesh14->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh14->set_backface_normal_overlay_texture_index(mesh14->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // grid2
    mesh15->set_material(env_mapped_material_fast);
    mesh15->set_reflect_to_refract_ratio(0.33); // 33% reflective

    return 1;
}

void onIdle()
{
    glutPostRedisplay();
}

void onTick()
{
    static unsigned int prev_tick = 0;
    static unsigned int frames = 0;
    unsigned int tick = glutGet(GLUT_ELAPSED_TIME);
    unsigned int delta_time = tick-prev_tick;
    static float fps = 0;
    if(delta_time > 1000) {
        fps = 1000.0*frames/delta_time;
        frames = 0;
        prev_tick = tick;
    }
    if(show_fps && delta_time > 100) {
        std::stringstream ss;
        ss << std::setprecision(2) << std::fixed << fps << " FPS, "
            << "Mouse: {" << mouse_drag.x << ", " << mouse_drag.y << "}, "
            << "Yaw=" << ORIENT_YAW(orient) << ", Pitch=" << ORIENT_PITCH(orient) << ", Radius=" << orbit_radius << ", "
            << "Zoom=" << zoom;
        glutSetWindowTitle(ss.str().c_str());
    }
    frames++;

    angle = static_cast<float>(glutGet(GLUT_ELAPSED_TIME))/1000*15; // base 15 degrees per second
}

void onDisplay()
{
    onTick();

    mesh2->set_orient(glm::vec3(angle*3, angle*2, angle*4));
    mesh_apply_ripple(mesh15, glm::vec3(0.5, 0, 0.5), 0.1, 0.5, -angle*0.1);
    mesh15->update_buffers();

    vt::Scene *scene = vt::Scene::instance();

    frontface_depth_overlay_fb->bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    scene->render(false, false);
    frontface_depth_overlay_fb->unbind();

    backface_depth_overlay_fb->bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    scene->render(false, false);
    glCullFace(GL_BACK);
    backface_depth_overlay_fb->unbind();

    backface_normal_overlay_fb->bind();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);
    scene->render(false, false, true);
    glCullFace(GL_BACK);
    backface_normal_overlay_fb->unbind();

    if(post_process_blur) {
        // switch to write-through mode to perform downsampling
        mesh_overlay->set_material(overlay_write_through_material);

        // render-to-texture for initial input texture
        hi_res_color_overlay_fb->bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene->render(false, true);
        hi_res_color_overlay_fb->unbind();

        // linear downsample texture from hi-res to med-res
        mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("hi_res_color_overlay"));
        med_res_color_overlay_fb->bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene->render(true);
        med_res_color_overlay_fb->unbind();

        // linear downsample texture from med-res to lo-res
        mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("med_res_color_overlay"));
        lo_res_color_overlay_fb->bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene->render(true);
        lo_res_color_overlay_fb->unbind();

        // switch to blur mode to apply bloom filter
        mesh_overlay->set_material(overlay_blur_material);

        // blur texture in low-res
        mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("lo_res_color_overlay"));
        for(int i = 0; i < BLUR_ITERS; i++) {
            lo_res_color_overlay_fb->bind();
            //glClearColor(0, 0, 0, 1);
            //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            scene->render(true);
            lo_res_color_overlay_fb->unbind();
        }

        // switch to write-through mode to merge blurred texture with hi-res texture
        mesh_overlay->set_material(overlay_bloom_material);

        hi_res_color_overlay_fb->bind();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        scene->render(true);
        hi_res_color_overlay_fb->unbind();

        // switch to write-through mode to display final output texture
        mesh_overlay->set_material(overlay_write_through_material);
    }

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    scene->render(post_process_blur || (overlay_mode != 0));

//    stencil_fb->bind();
//    glEnable(GL_STENCIL_TEST);
//
//    glClearColor(0, 0, 0, 1);
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
//
//    glStencilFunc(GL_ALWAYS, 0x1, 0x1);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
//    scene->render();
//
//    glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
//    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
//    glDisable(GL_LIGHTING);
//    glColor3f(0,1,0); // outline color
//    glLineWidth(5);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
//    scene->render();
//    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // return to fill mode
//    glLineWidth(1);
//    glEnable(GL_LIGHTING);
//
//    glDisable(GL_STENCIL_TEST);
//    stencil_fb->unbind();

    if(show_vert_normals) {
        scene->render_vert_normals();
    }
    if(show_lights) {
        scene->render_lights();
    }
    glutSwapBuffers();
}

void set_mesh_visibility(bool visible)
{
    mesh->set_visible(visible);    // box
    //mesh2->set_visible(visible); // box2
    //mesh3->set_visible(visible); // grid
    mesh4->set_visible(visible);   // sphere
    mesh5->set_visible(visible);   // torus
    mesh6->set_visible(visible);   // cylinder
    mesh7->set_visible(visible);   // cone
    mesh8->set_visible(visible);   // hemisphe
    mesh9->set_visible(visible);   // tetrahed
    mesh10->set_visible(visible);  // diamond
    mesh11->set_visible(visible);  // box3
    mesh12->set_visible(visible);  // diamond2
    mesh13->set_visible(visible);  // sphere2
    mesh14->set_visible(visible);  // box4
    mesh15->set_visible(visible);  // grid2
}

void onKeyboard(unsigned char key, int x, int y)
{
    switch(key) {
        case 'b': // blur
            post_process_blur = !post_process_blur;
            if(post_process_blur) {
                mesh_overlay->set_material(overlay_blur_material);
            } else {
                mesh_overlay->set_material(overlay_write_through_material);
            }
            break;
        case 'd': // demo
            if(demo_mode == 0) {
                set_mesh_visibility(false);
                mesh12->set_visible(true); // diamond2
                demo_mode = 1;
            } else if(demo_mode == 1) {
                set_mesh_visibility(false);
                mesh13->set_visible(true); // sphere2
                demo_mode = 2;
            } else if(demo_mode == 2) {
                set_mesh_visibility(false);
                mesh14->set_visible(true); // box4
                demo_mode = 3;
            } else if(demo_mode == 3) {
                set_mesh_visibility(false);
                mesh15->set_visible(true); // grid2
                demo_mode = 4;
            } else if(demo_mode == 4) {
                set_mesh_visibility(true);
                mesh12->set_visible(false); // diamond2
                mesh13->set_visible(false); // sphere2
                mesh14->set_visible(false); // box4
                mesh15->set_visible(false); // grid2
                demo_mode = 0;
            }
            break;
        case 'f': // frame rate
            show_fps = !show_fps;
            if(!show_fps) {
                glutSetWindowTitle(DEFAULT_CAPTION);
            }
            break;
        case 'l': // lights
            show_lights = !show_lights;
            break;
        case 'n': // normals
            show_vert_normals = !show_vert_normals;
            break;
        case 'o': // overlay
            if(overlay_mode == 0) {
                mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("backface_normal_overlay"));
                overlay_mode = 1;
            } else if(overlay_mode == 1) {
                mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
                overlay_mode = 2;
            } else if(overlay_mode == 2) {
                mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("backface_depth_overlay"));
                overlay_mode = 3;
            } else if(overlay_mode == 3) {
                mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("hi_res_color_overlay"));
                overlay_mode = 0;
            }
            break;
        case 'p': // projection
            if(camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_PERSPECTIVE) {
                camera->set_projection_mode(vt::Camera::PROJECTION_MODE_ORTHO);
            } else if(camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_ORTHO) {
                camera->set_projection_mode(vt::Camera::PROJECTION_MODE_PERSPECTIVE);
            }
            break;
        case 't': // texture
            if(texture_index == 0) {
                texture_index = 1; // GL_TEXTURE1
            } else if(texture_index == 1) {
                texture_index = 2; // GL_TEXTURE2
            } else if(texture_index == 2) {
                texture_index = 3; // GL_TEXTURE3
            } else if(texture_index == 3) {
                texture_index = 0; // GL_TEXTURE0
            }
            mesh->set_texture_index( texture_index);
            mesh2->set_texture_index(texture_index);
            //mesh3->set_texture_index(texture_index);
            //mesh4->set_texture_index(texture_index);
            //mesh5->set_texture_index(texture_index);
            //mesh6->set_texture_index(texture_index);
            //mesh7->set_texture_index(texture_index);
            break;
        case 'w': // wireframe
            wire_frame_mode = !wire_frame_mode;
            if(wire_frame_mode) {
                glPolygonMode(GL_FRONT, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT, GL_FILL);
            }
            break;
        case 27: // escape
            exit(0);
            break;
    }
}

void onSpecial(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_F1:
            light->set_enabled(!light->get_enabled());
            break;
        case GLUT_KEY_F2:
            light2->set_enabled(!light2->get_enabled());
            break;
        case GLUT_KEY_F3:
            light3->set_enabled(!light3->get_enabled());
            break;
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            break;
    }
}

void onSpecialUp(int key, int x, int y)
{
    switch(key) {
        case GLUT_KEY_F1:
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            break;
    }
}

void onMouse(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN) {
        prev_mouse_coord.x = x;
        prev_mouse_coord.y = y;
        if(button == GLUT_LEFT_BUTTON) {
            left_mouse_down = true;
            prev_orient = orient;
        }
        if(button == GLUT_RIGHT_BUTTON) {
            right_mouse_down = true;
            if(camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_PERSPECTIVE) {
                prev_orbit_radius = orbit_radius;
            } else if (camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_ORTHO) {
                prev_zoom = zoom;
            }
        }
    }
    else {
        left_mouse_down = right_mouse_down = false;
    }
}

void onMotion(int x, int y)
{
    if(left_mouse_down || right_mouse_down) {
        mouse_drag = glm::vec2(x, y)-prev_mouse_coord;
    }
    if(left_mouse_down) {
        orient = prev_orient+glm::vec3(0, mouse_drag.y*ORIENT_PITCH(orbit_speed), mouse_drag.x*ORIENT_YAW(orbit_speed));
        camera->orbit(orient, orbit_radius);
    }
    if(right_mouse_down) {
        if(camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_PERSPECTIVE) {
            orbit_radius = prev_orbit_radius+mouse_drag.y*dolly_speed;
            camera->orbit(orient, orbit_radius);
        } else if (camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_ORTHO) {
            zoom = prev_zoom+mouse_drag.y*ortho_dolly_speed;
            camera->set_zoom(zoom);
        }
    }
}

void onReshape(int width, int height)
{
    glViewport(0, 0, width, height);
    camera->resize_viewport(width, height);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH/*|GLUT_STENCIL*/);
    glutInitWindowSize(init_screen_width, init_screen_height);
    glutCreateWindow(DEFAULT_CAPTION);

    GLenum glew_status = glewInit();
    if(glew_status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        return 1;
    }

    if(!GLEW_VERSION_2_0) {
        fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
        return 1;
    }

    char* s = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("GLSL version %s\n", s);

    if(init_resources()) {
        glutDisplayFunc(onDisplay);
        glutKeyboardFunc(onKeyboard);
        glutSpecialFunc(onSpecial);
        glutSpecialUpFunc(onSpecialUp);
        glutMouseFunc(onMouse);
        glutMotionFunc(onMotion);
        glutReshapeFunc(onReshape);
        glutIdleFunc(onIdle);
        //glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glutMainLoop();
    }

    return 0;
}
