/**
 * From the OpenGL Programming wikibook: http://en.wikibooks.org/wiki/OpenGL_Programming
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 * Enhanced by: Jerry Chen
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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
#include <File3ds.h>
#include <FrameBuffer.h>
#include <Light.h>
#include <Modifiers.h>
#include <Material.h>
#include <Mesh.h>
#include <PrimitiveFactory.h>
#include <Program.h>
#include <Scene.h>
#include <Shader.h>
#include <ShaderContext.h>
#include <Texture.h>
#include <Util.h>
#include <VarAttribute.h>
#include <VarUniform.h>
#include <vector>
#include <iostream> // std::cout
#include <sstream> // std::stringstream
#include <iomanip> // std::setprecision
#include <unistd.h> // access

#define HI_RES_TEX_DIM  512
#define MED_RES_TEX_DIM 256
#define LO_RES_TEX_DIM  128
#define BLUR_ITERS      5
#define RAND_TEX_DIM    8

enum demo_mode_t {
    DEMO_MODE_DEFAULT,
    DEMO_MODE_DIAMOND,
    DEMO_MODE_SPHERE,
    DEMO_MODE_BOX,
    DEMO_MODE_GRID,
    DEMO_MODE_MESHES_IMPORTED,
    DEMO_MODE_COUNT
};

enum overlay_mode_t {
    OVERLAY_MODE_DEFAULT,
    OVERLAY_MODE_FF_DEPTH,
    OVERLAY_MODE_BF_DEPTH,
    OVERLAY_MODE_FF_NORMAL,
    OVERLAY_MODE_BF_NORMAL,
    OVERLAY_MODE_SSAO,
    OVERLAY_MODE_COUNT
};

const char* DEFAULT_CAPTION = "";

int init_screen_width  = 800,
    init_screen_height = 600;
vt::Camera* camera = NULL;
vt::Mesh *mesh_skybox  = NULL,
         *mesh_overlay = NULL,
         *mesh         = NULL,
         *mesh2        = NULL,
         *mesh3        = NULL,
         *mesh4        = NULL,
         *mesh5        = NULL,
         *mesh6        = NULL,
         *mesh7        = NULL,
         *mesh8        = NULL,
         *mesh9        = NULL,
         *mesh10       = NULL,
         *hidden_mesh  = NULL,
         *hidden_mesh2 = NULL,
         *hidden_mesh3 = NULL,
         *hidden_mesh4 = NULL;
std::vector<vt::Mesh*> meshes_imported;
vt::Light *light  = NULL,
          *light2 = NULL,
          *light3 = NULL;
vt::Texture *texture                            = NULL,
            *texture2                           = NULL,
            *texture3                           = NULL,
            *texture4                           = NULL,
            *texture5                           = NULL,
            *frontface_depth_overlay_texture    = NULL,
            *forward_prop_overlay_texture       = NULL,
            *forward_prop_input_vec_texture     = NULL,
            *forward_prop_input_weights_texture = NULL,
            *backface_depth_overlay_texture     = NULL,
            *frontface_normal_overlay_texture   = NULL,
            *backface_normal_overlay_texture    = NULL,
            *ssao_overlay_texture               = NULL,
            *hi_res_color_overlay_texture       = NULL,
            *med_res_color_overlay_texture      = NULL,
            *lo_res_color_overlay_texture       = NULL,
            *random_texture                     = NULL;

vt::FrameBuffer *frontface_depth_overlay_fb  = NULL,
                *forward_prop_overlay_fb     = NULL,
                *backface_depth_overlay_fb   = NULL,
                *frontface_normal_overlay_fb = NULL,
                *backface_normal_overlay_fb  = NULL,
                *ssao_overlay_fb             = NULL,
                *hi_res_color_overlay_fb     = NULL,
                *med_res_color_overlay_fb    = NULL,
                *lo_res_color_overlay_fb     = NULL;

bool left_mouse_down  = false,
     right_mouse_down = false;
glm::vec2 prev_mouse_coord,
          mouse_drag;
glm::vec3 prev_euler,
          euler,
          orbit_speed = glm::vec3(0, -0.5, -0.5);
float prev_orbit_radius = 0,
      orbit_radius      = 8,
      dolly_speed       = 0.1,
      light_distance    = 4;
bool show_bbox         = false,
     show_fps          = false,
     show_lights       = false,
     show_normals      = false,
     wireframe_mode    = false,
     show_guide_wires  = false,
     show_paths        = true,
     show_axis         = false,
     show_axis_labels  = false,
     show_diamond      = false,
     post_process_blur = true,
     do_animation      = true;

int texture_id = 0;
float prev_zoom         = 0,
      zoom              = 1,
      ortho_dolly_speed = 0.1;

int overlay_mode = OVERLAY_MODE_DEFAULT,
    demo_mode    = DEMO_MODE_DEFAULT;

float phase = 0;

vt::Material *overlay_write_through_material = NULL,
             *overlay_bloom_filter_material  = NULL,
             *overlay_max_material           = NULL,
             *overlay_forward_prop_material  = NULL;

int init_resources()
{
    vt::Scene* scene = vt::Scene::instance();

    mesh_skybox = vt::PrimitiveFactory::create_viewport_quad("grid");
    scene->set_skybox(mesh_skybox);

    mesh_overlay = vt::PrimitiveFactory::create_viewport_quad("grid3");
    scene->set_overlay(mesh_overlay);

    scene->add_mesh(mesh         = vt::PrimitiveFactory::create_box(                  "box"));
    scene->add_mesh(mesh2        = vt::PrimitiveFactory::create_grid(                 "grid",       1,  1,   10, 10, 0.05, 0.05));
    scene->add_mesh(mesh3        = vt::PrimitiveFactory::create_sphere(               "sphere",     16, 16,  0.5));
    scene->add_mesh(mesh4        = vt::PrimitiveFactory::create_torus(                "torus",      16, 16,  0.5, 0.25));
    scene->add_mesh(mesh5        = vt::PrimitiveFactory::create_cylinder(             "cylinder",   16, 0.5, 1));
    scene->add_mesh(mesh6        = vt::PrimitiveFactory::create_cone(                 "cone",       16, 0.5, 1));
    scene->add_mesh(mesh7        = vt::PrimitiveFactory::create_hemisphere(           "hemisphere", 16, 16,  0.5));
    scene->add_mesh(mesh8        = vt::PrimitiveFactory::create_tetrahedron(          "tetrahedron"));
    scene->add_mesh(mesh9        = vt::PrimitiveFactory::create_diamond_brilliant_cut("diamond"));
    scene->add_mesh(mesh10       = vt::PrimitiveFactory::create_box(                  "box2"));
    scene->add_mesh(hidden_mesh  = vt::PrimitiveFactory::create_diamond_brilliant_cut("diamond2"));
    scene->add_mesh(hidden_mesh2 = vt::PrimitiveFactory::create_sphere(               "sphere2", 16, 16, 0.5));
    scene->add_mesh(hidden_mesh3 = vt::PrimitiveFactory::create_box(                  "box3"));
    scene->add_mesh(hidden_mesh4 = vt::PrimitiveFactory::create_grid(                 "grid2",   32, 32, 1, 1));
    const char* model_filename = "data/star_wars/TI_Low0.3ds";
    if(access(model_filename, F_OK) != -1) {
        vt::File3ds::load3ds(model_filename, -1, &meshes_imported);
    }
    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
        scene->add_mesh(*p);
    }

    mesh->set_origin(        glm::vec3(-0.5, -0.5, -0.5)); // box
    mesh2->set_origin(       glm::vec3(-5, 5, -1));        // grid
    mesh3->set_origin(       glm::vec3(2, 0, 0));          // sphere
    mesh4->set_origin(       glm::vec3(-2, 0, 0));         // torus
    mesh5->set_origin(       glm::vec3(0, -2.5, 0));       // cylinder
    mesh6->set_origin(       glm::vec3(0, 1.5, 0));        // cone
    mesh7->set_origin(       glm::vec3(-2, 1.75, 0));      // hemisphere
    mesh8->set_origin(       glm::vec3(1.5, 1.5, -0.5));   // tetrahedron
    mesh9->set_origin(       glm::vec3(-2, -2.5, 0));      // diamond
    mesh10->set_origin(      glm::vec3(1.5, -2.5, -0.5));  // box2
    hidden_mesh->set_origin( glm::vec3(0, -1, 0));         // diamond2
    hidden_mesh2->set_origin(glm::vec3(0, 0, 0));          // sphere2
    hidden_mesh3->set_origin(glm::vec3(-1, -1, -1));       // box3
    hidden_mesh4->set_origin(glm::vec3(-2, 0, -2));        // grid2
    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
        (*p)->set_origin(glm::vec3(0, 0, 0));
    }

    mesh2->set_euler(glm::vec3(0, 90, 0));

    mesh2->set_visible(false);
    //mesh5->set_visible(false);
    //mesh6->set_visible(false);
    hidden_mesh->set_visible(false);
    hidden_mesh2->set_visible(false);
    hidden_mesh3->set_visible(false);
    hidden_mesh4->set_visible(false);
    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
        (*p)->set_visible(false);
    }

    hidden_mesh->set_scale(glm::vec3( 2, 2, 2)); // diamond2
    hidden_mesh2->set_scale(glm::vec3(2, 2, 2)); // sphere2
    hidden_mesh3->set_scale(glm::vec3(2, 2, 2)); // box3
    hidden_mesh4->set_scale(glm::vec3(4, 4, 4)); // grid2
    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
        (*p)->set_scale(glm::vec3(0.1, 0.1, 0.1));
    }

    vt::Material* bump_mapped_material = new vt::Material("bump_mapped",
                                                          "src/shaders/bump_mapped.v.glsl",
                                                          "src/shaders/bump_mapped.f.glsl");
    scene->add_material(bump_mapped_material);

    vt::Material* phong_material = new vt::Material("phong",
                                                    "src/shaders/phong.v.glsl",
                                                    "src/shaders/phong.f.glsl");
    scene->add_material(phong_material);

    vt::Material* ssao_material = new vt::Material("ssao",
                                                   "src/shaders/ssao.v.glsl",
                                                   "src/shaders/ssao.f.glsl");
    scene->add_material(ssao_material);
    scene->set_ssao_material(ssao_material);

    vt::Material* skybox_material = new vt::Material("skybox",
                                                     "src/shaders/skybox.v.glsl",
                                                     "src/shaders/skybox.f.glsl",
                                                     true); // use_overlay
    scene->add_material(skybox_material);

    overlay_write_through_material = new vt::Material("overlay_write_through",
                                                      "src/shaders/overlay_write_through.v.glsl",
                                                      "src/shaders/overlay_write_through.f.glsl",
                                                      true); // use_overlay
    scene->add_material(overlay_write_through_material);

    overlay_bloom_filter_material = new vt::Material("overlay_bloom_filter",
                                                     "src/shaders/overlay_bloom_filter.v.glsl",
                                                     "src/shaders/overlay_bloom_filter.f.glsl",
                                                     true); // use_overlay
    scene->add_material(overlay_bloom_filter_material);

    overlay_max_material = new vt::Material("overlay_max",
                                            "src/shaders/overlay_max.v.glsl",
                                            "src/shaders/overlay_max.f.glsl",
                                            true); // use_overlay
    scene->add_material(overlay_max_material);

    overlay_forward_prop_material = new vt::Material("overlay_forward_prop",
                                                     "src/shaders/overlay_forward_prop.v.glsl",
                                                     "src/shaders/overlay_forward_prop.f.glsl",
                                                     true); // use_overlay
    scene->add_material(overlay_forward_prop_material);

    vt::Material* texture_mapped_material = new vt::Material("texture_mapped",
                                                             "src/shaders/texture_mapped.v.glsl",
                                                             "src/shaders/texture_mapped.f.glsl");
    scene->add_material(texture_mapped_material);

    vt::Material* env_mapped_material = new vt::Material("env_mapped",
                                                         "src/shaders/env_mapped.v.glsl",
                                                         "src/shaders/env_mapped.f.glsl");
    scene->add_material(env_mapped_material);

    vt::Material* env_mapped_dbl_refract_material = new vt::Material("env_mapped_dbl_refract",
                                                                     "src/shaders/env_mapped_dbl_refract.v.glsl",
                                                                     "src/shaders/env_mapped_dbl_refract.f.glsl");
    scene->add_material(env_mapped_dbl_refract_material);

    vt::Material* env_mapped_fast_material = new vt::Material("env_mapped_fast",
                                                              "src/shaders/env_mapped_fast.v.glsl",
                                                              "src/shaders/env_mapped_fast.f.glsl");
    scene->add_material(env_mapped_fast_material);

    vt::Material* normal_material = new vt::Material("normal",
                                                     "src/shaders/normal.v.glsl",
                                                     "src/shaders/normal.f.glsl");
    scene->add_material(normal_material);

    vt::Material* normal_fast_material = new vt::Material("normal_fast",
                                                          "src/shaders/normal_fast.v.glsl",
                                                          "src/shaders/normal_fast.f.glsl");
    scene->add_material(normal_fast_material);
    scene->set_normal_material(normal_fast_material);

    vt::Material* ambient_material = new vt::Material("ambient",
                                                      "src/shaders/ambient.v.glsl",
                                                      "src/shaders/ambient.f.glsl");
    scene->add_material(ambient_material);
    scene->set_wireframe_material(ambient_material);

    texture = new vt::Texture("dex3d",
                              vt::Texture::RGBA,
                              glm::ivec2(res_texture.width, res_texture.height),
                              false,
                              vt::Texture::RGB,
                              res_texture.pixel_data);
    scene->add_texture(                  texture);
    bump_mapped_material->add_texture(   texture);
    texture_mapped_material->add_texture(texture);

    texture2 = new vt::Texture("lode_runner",
                               vt::Texture::RGBA,
                               glm::ivec2(res_texture2.width, res_texture2.height),
                               true,
                               vt::Texture::RGB,
                               res_texture2.pixel_data);
    scene->add_texture(               texture2);
    bump_mapped_material->add_texture(texture2);

    texture3 = new vt::Texture("chesterfield_color",
                               "data/chesterfield_color.png");
    scene->add_texture(                          texture3);
    bump_mapped_material->add_texture(           texture3);
    env_mapped_material->add_texture(            texture3);
    env_mapped_dbl_refract_material->add_texture(texture3);

    texture4 = new vt::Texture("chesterfield_normal",
                               "data/chesterfield_normal.png");
    scene->add_texture(                          texture4);
    bump_mapped_material->add_texture(           texture4);
    env_mapped_material->add_texture(            texture4);
    env_mapped_dbl_refract_material->add_texture(texture4);
    normal_material->add_texture(                texture4);

    texture5 = new vt::Texture("skybox_texture",
                               "data/SaintPetersSquare2/posx.png",
                               "data/SaintPetersSquare2/negx.png",
                               "data/SaintPetersSquare2/posy.png",
                               "data/SaintPetersSquare2/negy.png",
                               "data/SaintPetersSquare2/posz.png",
                               "data/SaintPetersSquare2/negz.png");
    scene->add_texture(                          texture5);
    skybox_material->add_texture(                texture5);
    env_mapped_material->add_texture(            texture5);
    env_mapped_dbl_refract_material->add_texture(texture5);
    env_mapped_fast_material->add_texture(       texture5);

    frontface_depth_overlay_texture = new vt::Texture("frontface_depth_overlay",
                                                      vt::Texture::DEPTH,
                                                      glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    texture_mapped_material->add_texture(        frontface_depth_overlay_texture);
    env_mapped_dbl_refract_material->add_texture(frontface_depth_overlay_texture);
    overlay_write_through_material->add_texture( frontface_depth_overlay_texture);
    overlay_bloom_filter_material->add_texture(  frontface_depth_overlay_texture);
    ssao_material->add_texture(                  frontface_depth_overlay_texture);

    forward_prop_overlay_texture = new vt::Texture("forward_prop",
                                                   vt::Texture::RED,
                                                   glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    overlay_forward_prop_material->add_texture(forward_prop_overlay_texture);
    overlay_write_through_material->add_texture(forward_prop_overlay_texture);

    forward_prop_input_vec_texture = new vt::Texture("forward_prop_input_vec",
                                                     vt::Texture::RED,
                                                     glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    overlay_forward_prop_material->add_texture(forward_prop_input_vec_texture);
    overlay_write_through_material->add_texture(forward_prop_input_vec_texture);

    forward_prop_input_weights_texture = new vt::Texture("forward_prop_input_weights",
                                                         vt::Texture::RED,
                                                         glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    overlay_forward_prop_material->add_texture(forward_prop_input_weights_texture);
    overlay_write_through_material->add_texture(forward_prop_input_weights_texture);

    backface_depth_overlay_texture = new vt::Texture("backface_depth_overlay",
                                                     vt::Texture::DEPTH,
                                                     glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    texture_mapped_material->add_texture(        backface_depth_overlay_texture);
    env_mapped_dbl_refract_material->add_texture(backface_depth_overlay_texture);
    overlay_write_through_material->add_texture( backface_depth_overlay_texture);
    overlay_bloom_filter_material->add_texture(  backface_depth_overlay_texture);

    frontface_normal_overlay_texture = new vt::Texture("frontface_normal_overlay",
                                                       vt::Texture::RGBA,
                                                       glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    texture_mapped_material->add_texture(        frontface_normal_overlay_texture);
    env_mapped_dbl_refract_material->add_texture(frontface_normal_overlay_texture);
    overlay_write_through_material->add_texture( frontface_normal_overlay_texture);
    overlay_bloom_filter_material->add_texture(  frontface_normal_overlay_texture);

    backface_normal_overlay_texture = new vt::Texture("backface_normal_overlay",
                                                      vt::Texture::RGBA,
                                                      glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    texture_mapped_material->add_texture(        backface_normal_overlay_texture);
    env_mapped_dbl_refract_material->add_texture(backface_normal_overlay_texture);
    overlay_write_through_material->add_texture( backface_normal_overlay_texture);
    overlay_bloom_filter_material->add_texture(  backface_normal_overlay_texture);

    ssao_overlay_texture = new vt::Texture("ssao_overlay",
                                           vt::Texture::RGBA,
                                           glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    texture_mapped_material->add_texture(        ssao_overlay_texture);
    env_mapped_dbl_refract_material->add_texture(ssao_overlay_texture);
    overlay_write_through_material->add_texture( ssao_overlay_texture);
    overlay_bloom_filter_material->add_texture(  ssao_overlay_texture);

    hi_res_color_overlay_texture = new vt::Texture("hi_res_color_overlay",
                                                   vt::Texture::RGBA,
                                                   glm::ivec2(HI_RES_TEX_DIM, HI_RES_TEX_DIM));
    texture_mapped_material->add_texture(       hi_res_color_overlay_texture);
    overlay_write_through_material->add_texture(hi_res_color_overlay_texture);
    overlay_bloom_filter_material->add_texture( hi_res_color_overlay_texture);
    overlay_max_material->add_texture(          hi_res_color_overlay_texture);

    med_res_color_overlay_texture = new vt::Texture("med_res_color_overlay",
                                                    vt::Texture::RGBA,
                                                    glm::ivec2(MED_RES_TEX_DIM, MED_RES_TEX_DIM));
    texture_mapped_material->add_texture(       med_res_color_overlay_texture);
    overlay_write_through_material->add_texture(med_res_color_overlay_texture);
    overlay_bloom_filter_material->add_texture( med_res_color_overlay_texture);

    lo_res_color_overlay_texture = new vt::Texture("lo_res_color_overlay",
                                                   vt::Texture::RGBA,
                                                   glm::ivec2(LO_RES_TEX_DIM, LO_RES_TEX_DIM));
    texture_mapped_material->add_texture(       lo_res_color_overlay_texture);
    overlay_write_through_material->add_texture(lo_res_color_overlay_texture);
    overlay_bloom_filter_material->add_texture( lo_res_color_overlay_texture);
    overlay_max_material->add_texture(          lo_res_color_overlay_texture);

    random_texture = new vt::Texture("random_texture",
                                     vt::Texture::RGBA,
                                     glm::ivec2(RAND_TEX_DIM, RAND_TEX_DIM),
                                     false,
                                     vt::Texture::RGBA,
                                     NULL);
    random_texture->randomize();
    texture_mapped_material->add_texture(random_texture);
    ssao_material->add_texture(          random_texture);

    glm::vec3 origin = glm::vec3();
    camera = new vt::Camera("camera", origin + glm::vec3(0, 0, orbit_radius), origin);
    scene->set_camera(camera);

    frontface_depth_overlay_fb  = new vt::FrameBuffer(frontface_depth_overlay_texture, camera);
    forward_prop_overlay_fb     = new vt::FrameBuffer(forward_prop_overlay_texture, camera);
    backface_depth_overlay_fb   = new vt::FrameBuffer(backface_depth_overlay_texture, camera);
    frontface_normal_overlay_fb = new vt::FrameBuffer(frontface_normal_overlay_texture, camera);
    backface_normal_overlay_fb  = new vt::FrameBuffer(backface_normal_overlay_texture, camera);
    ssao_overlay_fb             = new vt::FrameBuffer(ssao_overlay_texture, camera);
    hi_res_color_overlay_fb     = new vt::FrameBuffer(hi_res_color_overlay_texture, camera);
    med_res_color_overlay_fb    = new vt::FrameBuffer(med_res_color_overlay_texture, camera);
    lo_res_color_overlay_fb     = new vt::FrameBuffer(lo_res_color_overlay_texture, camera);

    scene->add_light(light  = new vt::Light("light1", origin + glm::vec3(light_distance, 0, 0), glm::vec3(1, 0, 0)));
    scene->add_light(light2 = new vt::Light("light2", origin + glm::vec3(0, light_distance, 0), glm::vec3(0, 1, 0)));
    scene->add_light(light3 = new vt::Light("light3", origin + glm::vec3(0, 0, light_distance), glm::vec3(0, 0, 1)));

    mesh_skybox->set_material(skybox_material);
    mesh_skybox->set_texture_index(mesh_skybox->get_material()->get_texture_index_by_name("skybox_texture"));

    mesh_overlay->set_material(overlay_write_through_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("hi_res_color_overlay"));

    // box
    mesh->set_material(bump_mapped_material);
    mesh->set_texture_index(     mesh->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh->set_bump_texture_index(mesh->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh->set_ambient_color(glm::vec3(0,0,0));

    // grid
    mesh2->set_material(ambient_material);
    mesh2->set_ambient_color(glm::vec3(0,0,0));

    // sphere
    mesh3->set_material(env_mapped_dbl_refract_material);
    mesh3->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh3->set_texture_index(                        mesh3->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh3->set_bump_texture_index(                   mesh3->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh3->set_frontface_depth_overlay_texture_index(mesh3->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh3->set_backface_depth_overlay_texture_index( mesh3->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh3->set_backface_normal_overlay_texture_index(mesh3->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // torus
    mesh4->set_material(env_mapped_material);
    mesh4->set_reflect_to_refract_ratio(1); // 100% reflective
    mesh4->set_texture_index(     mesh4->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh4->set_bump_texture_index(mesh4->get_material()->get_texture_index_by_name("chesterfield_normal"));

    // cylinder
    mesh5->set_material(texture_mapped_material);
    mesh5->set_texture_index(mesh5->get_material()->get_texture_index_by_name("dex3d"));

    // cone
    mesh6->set_material(normal_material);
    mesh6->set_bump_texture_index(mesh6->get_material()->get_texture_index_by_name("chesterfield_normal"));

    // hemisphere
    mesh7->set_material(env_mapped_fast_material);

    // tetrahedron
    mesh8->set_material(env_mapped_dbl_refract_material);
    mesh8->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh8->set_texture_index(                        mesh8->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh8->set_bump_texture_index(                   mesh8->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh8->set_frontface_depth_overlay_texture_index(mesh8->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh8->set_backface_depth_overlay_texture_index( mesh8->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh8->set_backface_normal_overlay_texture_index(mesh8->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // diamond
    mesh9->set_material(env_mapped_fast_material);
    mesh9->set_reflect_to_refract_ratio(0.33); // 33% reflective

    // box2
    mesh10->set_material(env_mapped_dbl_refract_material);
    mesh10->set_reflect_to_refract_ratio(0.33); // 33% reflective
    mesh10->set_texture_index(                        mesh10->get_material()->get_texture_index_by_name("chesterfield_color"));
    mesh10->set_bump_texture_index(                   mesh10->get_material()->get_texture_index_by_name("chesterfield_normal"));
    mesh10->set_frontface_depth_overlay_texture_index(mesh10->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    mesh10->set_backface_depth_overlay_texture_index( mesh10->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    mesh10->set_backface_normal_overlay_texture_index(mesh10->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // diamond2
    hidden_mesh->set_material(env_mapped_dbl_refract_material);
    hidden_mesh->set_reflect_to_refract_ratio(0.33); // 33% reflective
    hidden_mesh->set_texture_index(                        hidden_mesh->get_material()->get_texture_index_by_name("chesterfield_color"));
    hidden_mesh->set_bump_texture_index(                   hidden_mesh->get_material()->get_texture_index_by_name("chesterfield_normal"));
    hidden_mesh->set_frontface_depth_overlay_texture_index(hidden_mesh->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    hidden_mesh->set_backface_depth_overlay_texture_index( hidden_mesh->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    hidden_mesh->set_backface_normal_overlay_texture_index(hidden_mesh->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // sphere2
    hidden_mesh2->set_material(env_mapped_dbl_refract_material);
    hidden_mesh2->set_reflect_to_refract_ratio(0.33); // 33% reflective
    hidden_mesh2->set_texture_index(                        hidden_mesh2->get_material()->get_texture_index_by_name("chesterfield_color"));
    hidden_mesh2->set_bump_texture_index(                   hidden_mesh2->get_material()->get_texture_index_by_name("chesterfield_normal"));
    hidden_mesh2->set_frontface_depth_overlay_texture_index(hidden_mesh2->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    hidden_mesh2->set_backface_depth_overlay_texture_index( hidden_mesh2->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    hidden_mesh2->set_backface_normal_overlay_texture_index(hidden_mesh2->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
        (*p)->set_material(env_mapped_fast_material);
        //(*p)->set_reflect_to_refract_ratio(0.33); // 33% reflective
    }

    // box3
    hidden_mesh3->set_material(env_mapped_dbl_refract_material);
    hidden_mesh3->set_reflect_to_refract_ratio(0.33); // 33% reflective
    hidden_mesh3->set_texture_index(                        hidden_mesh3->get_material()->get_texture_index_by_name("chesterfield_color"));
    hidden_mesh3->set_bump_texture_index(                   hidden_mesh3->get_material()->get_texture_index_by_name("chesterfield_normal"));
    hidden_mesh3->set_frontface_depth_overlay_texture_index(hidden_mesh3->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
    hidden_mesh3->set_backface_depth_overlay_texture_index( hidden_mesh3->get_material()->get_texture_index_by_name("backface_depth_overlay"));
    hidden_mesh3->set_backface_normal_overlay_texture_index(hidden_mesh3->get_material()->get_texture_index_by_name("backface_normal_overlay"));

    // grid2
    //hidden_mesh4->set_material(env_mapped_fast_material);
    hidden_mesh4->set_material(phong_material);
    //hidden_mesh4->set_reflect_to_refract_ratio(0.33); // 33% reflective
    hidden_mesh4->set_ambient_color(glm::vec3(0, 0, 0));

    return 1;
}

int deinit_resources()
{
    if(frontface_depth_overlay_fb)  { delete frontface_depth_overlay_fb; }
    if(forward_prop_overlay_fb)     { delete forward_prop_overlay_fb; }
    if(backface_depth_overlay_fb)   { delete backface_depth_overlay_fb; }
    if(frontface_normal_overlay_fb) { delete frontface_normal_overlay_fb; }
    if(backface_normal_overlay_fb)  { delete backface_normal_overlay_fb; }
    if(ssao_overlay_fb)             { delete ssao_overlay_fb; }
    if(hi_res_color_overlay_fb)     { delete hi_res_color_overlay_fb; }
    if(med_res_color_overlay_fb)    { delete med_res_color_overlay_fb; }
    if(lo_res_color_overlay_fb)     { delete lo_res_color_overlay_fb; }

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
    unsigned int delta_time = tick - prev_tick;
    static float fps = 0;
    if(delta_time > 1000) {
        fps = 1000.0 * frames / delta_time;
        frames = 0;
        prev_tick = tick;
    }
    if(show_fps && delta_time > 100) {
        std::stringstream ss;
        ss << std::setprecision(2) << std::fixed << fps << " FPS, "
            << "Mouse: {" << mouse_drag.x << ", " << mouse_drag.y << "}, "
            << "Yaw=" << EULER_YAW(euler) << ", Pitch=" << EULER_PITCH(euler) << ", Radius=" << orbit_radius << ", "
            << "Zoom=" << zoom;
        //ss << "Width=" << camera->get_width() << ", Width=" << camera->get_height();
        glutSetWindowTitle(ss.str().c_str());
    }
    frames++;

    phase = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000 * 15; // base 15 degrees per second

    mesh_apply_ripple(hidden_mesh4, glm::vec3(0.5, 0, 0.5), 0.1, 0.5, -phase * 0.1, false);
    hidden_mesh4->update_buffers();
}

void apply_bloom_filter(vt::Scene*       scene,
                        vt::Texture*     input_to_blur_texture,
                        int              blur_iters,
                        vt::Texture*     input_sharp_texture,
                        float            glow_cutoff_threshold,
                        vt::FrameBuffer* output_fb)
{
    vt::Mesh* mesh_overlay = scene->get_overlay();

    // switch to write-through mode to perform downsampling
    mesh_overlay->set_material(overlay_write_through_material);

    // linear downsample texture from hi-res to med-res
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(input_to_blur_texture));
    med_res_color_overlay_fb->bind();
    scene->render(false, true);
    med_res_color_overlay_fb->unbind();

    // linear downsample texture from med-res to lo-res
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(med_res_color_overlay_fb->get_texture()));
    lo_res_color_overlay_fb->bind();
    scene->render(false, true);
    lo_res_color_overlay_fb->unbind();

    // switch to bloom filter mode
    mesh_overlay->set_material(overlay_bloom_filter_material);

    // blur texture in low-res
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(lo_res_color_overlay_fb->get_texture()));
    lo_res_color_overlay_fb->bind();
    for(int i = 0; i < blur_iters; i++) {
        scene->render(false, true);
    }
    lo_res_color_overlay_fb->unbind();

    // switch to max mode to merge blurred low-res texture with hi-res texture
    mesh_overlay->set_material(overlay_max_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(lo_res_color_overlay_fb->get_texture()));
    mesh_overlay->set_texture2_index(mesh_overlay->get_material()->get_texture_index(input_sharp_texture));

    output_fb->bind();
    scene->set_glow_cutoff_threshold(glow_cutoff_threshold); // allow bloom only if summed rgb brighter than glow cutoff threshold
    scene->render(false, true);
    output_fb->unbind();

    // switch to write-through mode to display final output texture
    mesh_overlay->set_material(overlay_write_through_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(output_fb->get_texture()));
}

void bpnet_prop_forward(vt::Scene*       scene,
                        vt::Texture*     vin_texture,     // IN
                        vt::Texture*     weights_texture, // IN
                        vt::FrameBuffer* vout_fb)         // OUT
{
    vt::Mesh* mesh_overlay = scene->get_overlay();
    vt::Texture* output_texture = vout_fb->get_texture();

    vin_texture->set_color_r32f(0.5);
    vin_texture->update(); // upload to gpu

    weights_texture->set_color_r32f(1.5);
    weights_texture->update(); // upload to gpu

    //            layer0            layer1                layer2
    //
    // input0 --> [v00]------w00----[v01|e01]------w00----[v02|e02] --> target0
    //                \             / |     \             / |
    //                 \  *--w01---* w02     \  *--w01---* w02
    //                  \/           /        \/           /
    //                  /\          /         /\          /
    //                 /  *--w10---/         /  *--w10---/
    //                /           / \       /           / \
    // input1 --> [v10]------w11----[v11|e11]------w11----[v12|e12] --> target1
    //                          /     |               /     |
    //                    *----*     w12        *----*     w12
    //                   /           /         /           /
    //                  /   *-------*         /   *-------*
    //                 /   /                 /   /
    //                /   /                 /   /
    //            [v20]--*              [v21]--*
    //
    //            bias==1               bias==1

    // step 1. first layer // step 2+. any other layer
    // =================== // ========================
    // v00 = input1        // v01 = sigmoid(v00*w00 + v10*w01 + 1*w02)
    // v10 = input2        // v11 = sigmoid(v00*w10 + v10*w11 + 1*w12)
    // v20 = 1             // v21 = 1

    // input[3x1]
    //   |  layer0       layer0-1          layer1
    //   |
    //  \|/ v[3x1]       w[3x3]            v[3x1]
    // +---+---+---+     +---+---+---+     +---+---+---+
    // |v00|   |   |     |w00|w01|w02|--+  |   |v01|   |
    // +---+---+---+     +---+---+---+  |  +---+---+---+
    // |v10|   |   |     |w10|w11|w12|--+  |   |v11|   |
    // +---+---+---+     +---+---+---+  |  +---+---+---+
    // | 1 |   |   |     |   |   |   |  |  |   | 1 |   |<-bias row
    // +---+---+---+     +---+---+---+  |  +---+---+---+
    //   |                              |        ^
    //   +------------------------------+--------+
    //     step 2+

    // enter gpu kernel
    vout_fb->bind();
    mesh_overlay->set_material(overlay_forward_prop_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(vin_texture));
    mesh_overlay->set_texture2_index(mesh_overlay->get_material()->get_texture_index(weights_texture));
    scene->render(false, true);
    vout_fb->unbind();

    output_texture->refresh(); // download from gpu
    std::cout << output_texture->get_pixel_r32f(glm::ivec2(0, 0)) << std::endl;

    // switch to write-through mode to display final output texture
    mesh_overlay->set_material(overlay_write_through_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(vout_fb->get_texture()));
}

void bpnet_prop_backward(vt::Scene*       scene,
                         vt::Texture*     ein_texture,     // IN
                         vt::Texture*     weights_texture, // IN
                         vt::FrameBuffer* eout_fb)         // OUT
{
    vt::Mesh* mesh_overlay = scene->get_overlay();
    vt::Texture* output_texture = eout_fb->get_texture();

    ein_texture->set_color_r32f(0.5);
    ein_texture->update(); // upload to gpu

    weights_texture->set_color_r32f(1.5);
    weights_texture->update(); // upload to gpu

    //            layer0            layer1                layer2
    //
    // input0 --> [v00]------w00----[v01|e01]------w00----[v02|e02] --> target0
    //                \             / |     \             / |
    //                 \  *--w01---* w02     \  *--w01---* w02
    //                  \/           /        \/           /
    //                  /\          /         /\          /
    //                 /  *--w10---/         /  *--w10---/
    //                /           / \       /           / \
    // input1 --> [v10]------w11----[v11|e11]------w11----[v12|e12] --> target1
    //                          /     |               /     |
    //                    *----*     w12        *----*     w12
    //                   /           /         /           /
    //                  /   *-------*         /   *-------*
    //                 /   /                 /   /
    //                /   /                 /   /
    //            [v20]--*              [v21]--*
    //
    //            bias==1               bias==1

    // step 2+. any other layer                            // step 1. last layer
    // ========================                            // ==================
    // e01 = sigmoidPri(v01) * (e02*w00 + e02*w01 + 1*w02) // e02 = sigmoidPri(v02) * (target0 - v02)
    // e11 = sigmoidPri(v11) * (e12*w10 + e12*w11 + 1*w12) // e12 = sigmoidPri(v12) * (target1 - v12)

    // layer1            layer1-2          layer2    +-------------+
    //                                               |             |
    // e[3x1]            w[3x3]            e[3x1]   \|/            | step 1
    // +---+---+---+     +---+---+---+     +---+---+---+           |
    // |   |e01|   |  +--|w00|w01|w02|     |   |   |e02|           |
    // +---+---+---+  |  +---+---+---+     +---+---+---+           |
    // |   |e11|   |  +--|w10|w11|w12|     |   |   |e12|           |
    // +---+---+---+  |  +---+---+---+     +---+---+---+           |
    // |   |   |   |  |  |   |   |   |     |   |   |   |<-bias row |
    // +---+---+---+  |  +---+---+---+     +---+---+---+           |
    //       ^        |                              |             |
    //       +--------+--------------------------+---+             |
    //          step 2+                          |                 |
    //                                     r3,2  |   +-------------+--target[3x1]
    //                                     [3x1] |   |
    //                                     +---+---+---+
    //                                     |   |v01|v02|
    //                                     +---+---+---+
    //                                     |   |v11|v12|
    //                                     +---+---+---+
    //                                     |   | 1 | 1 |<-bias row
    //                                     +---+---+---+

    // enter gpu kernel
    eout_fb->bind();
    mesh_overlay->set_material(overlay_forward_prop_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(ein_texture));
    mesh_overlay->set_texture2_index(mesh_overlay->get_material()->get_texture_index(weights_texture));
    scene->render(false, true);
    eout_fb->unbind();

    output_texture->refresh(); // download from gpu
    std::cout << output_texture->get_pixel_r32f(glm::ivec2(0, 0)) << std::endl;

    // switch to write-through mode to display final output texture
    mesh_overlay->set_material(overlay_write_through_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(eout_fb->get_texture()));
}

void bpnet_adjust_weights(vt::Scene*       scene,
                          vt::Texture*     vin_texture,  // IN
                          vt::Texture*     eout_texture, // IN
                          vt::FrameBuffer* weights_fb)   // OUT
{
    vt::Mesh* mesh_overlay = scene->get_overlay();
    vt::Texture* output_texture = weights_fb->get_texture();

    vin_texture->set_color_r32f(0.5);
    vin_texture->update(); // upload to gpu

    eout_texture->set_color_r32f(1.5);
    eout_texture->update(); // upload to gpu

    //            layer0            layer1                layer2
    //
    // input0 --> [v00]------w00----[v01|e01]------w00----[v02|e02] --> target0
    //                \             / |     \             / |
    //                 \  *--w01---* w02     \  *--w01---* w02
    //                  \/           /        \/           /
    //                  /\          /         /\          /
    //                 /  *--w10---/         /  *--w10---/
    //                /           / \       /           / \
    // input1 --> [v10]------w11----[v11|e11]------w11----[v12|e12] --> target1
    //                          /     |               /     |
    //                    *----*     w12        *----*     w12
    //                   /           /         /           /
    //                  /   *-------*         /   *-------*
    //                 /   /                 /   /
    //                /   /                 /   /
    //            [v20]--*              [v21]--*
    //
    //            bias==1               bias==1

    // for all layers
    // ==============
    // w00 += v01 * e02
    // w01 += v11 * e02
    // w02 += 1   * e02
    // w10 += v01 * e12
    // w11 += v11 * e12
    // w12 += 1   * e12

    // layer1            layer1-2          layer2
    //
    // v[3x1]            w[3x3]            v[3x1]
    // +---+---+---+     +---+---+---+     +---+---+---+
    // |   |v01|   |     |w00|w01|w02|<-+  |   |   |e02|
    // +---+---+---+     +---+---+---+  |  +---+---+---+
    // |   |v11|   |     |w10|w11|w12|<-+  |   |   |e12|
    // +---+---+---+     +---+---+---+  |  +---+---+---+
    // |   | 1 |   |     |   |   |   |  |  |   |   |   |<-bias row
    // +---+---+---+     +---+---+---+  |  +---+---+---+
    //       |                          |            |
    //       +--------------------------+------------+

    // enter gpu kernel
    weights_fb->bind();
    mesh_overlay->set_material(overlay_forward_prop_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(vin_texture));
    mesh_overlay->set_texture2_index(mesh_overlay->get_material()->get_texture_index(eout_texture));
    scene->render(false, true);
    weights_fb->unbind();

    output_texture->refresh(); // download from gpu
    std::cout << output_texture->get_pixel_r32f(glm::ivec2(0, 0)) << std::endl;

    // switch to write-through mode to display final output texture
    mesh_overlay->set_material(overlay_write_through_material);
    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index(weights_fb->get_texture()));
}

void onDisplay()
{
    if(do_animation) {
        onTick();
    }

    vt::Scene* scene = vt::Scene::instance();

    frontface_depth_overlay_fb->bind();
    scene->render(true, false, false);
    frontface_depth_overlay_fb->unbind();

    if(overlay_mode == OVERLAY_MODE_FF_NORMAL) {
        frontface_normal_overlay_fb->bind();
        scene->render(true, false, false, vt::Scene::use_material_type_t::USE_NORMAL_MATERIAL);
        frontface_normal_overlay_fb->unbind();
    }

    if(overlay_mode == OVERLAY_MODE_SSAO) {
        // prepare input_to_blur_texture
        ssao_overlay_fb->bind();
        scene->render(true, false, false, vt::Scene::use_material_type_t::USE_SSAO_MATERIAL);
        ssao_overlay_fb->unbind();

        // prepare input_sharp_texture
        hi_res_color_overlay_fb->bind();
        scene->render();
        hi_res_color_overlay_fb->unbind();

        apply_bloom_filter(scene,
                           ssao_overlay_fb->get_texture(),         // input_to_blur_texture
                           BLUR_ITERS,                             // blur_iters
                           hi_res_color_overlay_fb->get_texture(), // input_sharp_texture
                           0,                                      // glow_cutoff_threshold
                           ssao_overlay_fb);                       // output_fb
    }

    glCullFace(GL_FRONT);

    backface_depth_overlay_fb->bind();
    scene->render(true, false, false);
    backface_depth_overlay_fb->unbind();

    backface_normal_overlay_fb->bind();
    scene->render(true, false, false, vt::Scene::use_material_type_t::USE_NORMAL_MATERIAL);
    backface_normal_overlay_fb->unbind();

    glCullFace(GL_BACK);

    if(post_process_blur) {
        // prepare input_to_blur_texture and input_sharp_texture
        hi_res_color_overlay_fb->bind();
        scene->render();
        hi_res_color_overlay_fb->unbind();

        apply_bloom_filter(scene,
                           hi_res_color_overlay_fb->get_texture(), // input_to_blur_texture
                           BLUR_ITERS,                             // blur_iters
                           hi_res_color_overlay_fb->get_texture(), // input_sharp_texture
                           0.75,                                   // glow_cutoff_threshold
                           hi_res_color_overlay_fb);               // output_fb
    }

#if 0
    bpnet_prop_forward(scene,
                    forward_prop_input_vec_texture,     // input_vec_texture
                    forward_prop_input_weights_texture, // input_weights_texture
                    forward_prop_overlay_fb);           // output_fb
#endif

    if(wireframe_mode) {
        scene->render(true, false, false, vt::Scene::use_material_type_t::USE_WIREFRAME_MATERIAL);
    } else {
        scene->render(true, post_process_blur || overlay_mode);
    }

    if(show_guide_wires || show_paths || show_axis || show_axis_labels || show_bbox || show_normals) {
        scene->render_lines_and_text(show_guide_wires, show_paths, show_axis, show_axis_labels, show_bbox, show_normals);
    }
    if(show_lights) {
        scene->render_lights();
    }
    glutSwapBuffers();
}

void set_mesh_visibility(bool visible)
{
    mesh->set_visible(visible);         // box
    //mesh2->set_visible(visible);      // grid
    mesh3->set_visible(visible);        // sphere
    mesh4->set_visible(visible);        // torus
    mesh5->set_visible(visible);        // cylinder
    mesh6->set_visible(visible);        // cone
    mesh7->set_visible(visible);        // hemisphe
    mesh8->set_visible(visible);        // tetrahed
    mesh9->set_visible(visible);        // diamond
    mesh10->set_visible(visible);       // box2
    hidden_mesh->set_visible(visible);  // diamond2
    hidden_mesh2->set_visible(visible); // sphere2
    hidden_mesh3->set_visible(visible); // box3
    hidden_mesh4->set_visible(visible); // grid2
    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
        (*p)->set_visible(visible);
    }
}

void onKeyboard(unsigned char key, int x, int y)
{
    switch(key) {
        case 'b': // bbox
            show_bbox = !show_bbox;
            break;
        case 'd': // demo
            demo_mode = (demo_mode + 1) % DEMO_MODE_COUNT;
            switch(demo_mode) {
                case DEMO_MODE_DEFAULT:
                    set_mesh_visibility(true);
                    hidden_mesh->set_visible(false);  // diamond2
                    hidden_mesh2->set_visible(false); // sphere2
                    hidden_mesh3->set_visible(false); // box3
                    hidden_mesh4->set_visible(false); // grid2
                    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
                        (*p)->set_visible(false);
                    }
                    break;
                case DEMO_MODE_DIAMOND:
                    set_mesh_visibility(false);
                    hidden_mesh->set_visible(true); // diamond2
                    break;
                case DEMO_MODE_SPHERE:
                    set_mesh_visibility(false);
                    hidden_mesh2->set_visible(true); // sphere2
                    break;
                case DEMO_MODE_BOX:
                    set_mesh_visibility(false);
                    hidden_mesh3->set_visible(true); // box3
                    break;
                case DEMO_MODE_GRID:
                    set_mesh_visibility(false);
                    hidden_mesh4->set_visible(true); // grid2
                    break;
                case DEMO_MODE_MESHES_IMPORTED:
                    set_mesh_visibility(false);
                    for(std::vector<vt::Mesh*>::iterator p = meshes_imported.begin(); p != meshes_imported.end(); p++) {
                        (*p)->set_visible(true);
                    }
                    break;
                default:
                    break;
            }
            break;
        case 'f': // frame rate
            show_fps = !show_fps;
            if(!show_fps) {
                glutSetWindowTitle(DEFAULT_CAPTION);
            }
            break;
        case 'g': // guide wires
            show_guide_wires = !show_guide_wires;
            break;
        case 'l': // lights
            show_lights = !show_lights;
            break;
        case 'n': // normals
            show_normals = !show_normals;
            break;
        case 'o': // overlay
            overlay_mode = (overlay_mode + 1) % OVERLAY_MODE_COUNT;
            if(post_process_blur) {
                post_process_blur = false;
                mesh_overlay->set_material(overlay_write_through_material);
            }
            switch(overlay_mode) {
                case OVERLAY_MODE_DEFAULT:
                    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("hi_res_color_overlay"));
                    break;
                case OVERLAY_MODE_FF_DEPTH:
                    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("frontface_depth_overlay"));
                    break;
                case OVERLAY_MODE_BF_DEPTH:
                    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("backface_depth_overlay"));
                    break;
                case OVERLAY_MODE_FF_NORMAL:
                    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("frontface_normal_overlay"));
                    break;
                case OVERLAY_MODE_BF_NORMAL:
                    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("backface_normal_overlay"));
                    break;
                case OVERLAY_MODE_SSAO:
                    mesh_overlay->set_texture_index(mesh_overlay->get_material()->get_texture_index_by_name("ssao_overlay"));
                    break;
                default:
                    break;
            }
            mesh2->set_visible(overlay_mode == OVERLAY_MODE_SSAO);
            break;
        case 'p': // projection
            if(camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_PERSPECTIVE) {
                camera->set_projection_mode(vt::Camera::PROJECTION_MODE_ORTHO);
            } else if(camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_ORTHO) {
                camera->set_projection_mode(vt::Camera::PROJECTION_MODE_PERSPECTIVE);
            }
            break;
        case 'r': // blur
            if(overlay_mode != OVERLAY_MODE_DEFAULT) {
                overlay_mode = OVERLAY_MODE_DEFAULT; // switch back to default overlay
                post_process_blur = true;
                mesh_overlay->set_material(overlay_bloom_filter_material);
                mesh2->set_visible(false);
                break;
            }
            post_process_blur = !post_process_blur;
            if(post_process_blur) {
                mesh_overlay->set_material(overlay_bloom_filter_material);
            } else {
                mesh_overlay->set_material(overlay_write_through_material);
            }
            break;
        case 's': // paths
            show_paths = !show_paths;
            break;
        case 't': // texture
            if(texture_id == 0) {
                texture_id = 1; // GL_TEXTURE1
            } else if(texture_id == 1) {
                texture_id = 2; // GL_TEXTURE2
            } else if(texture_id == 2) {
                texture_id = 3; // GL_TEXTURE3
            } else if(texture_id == 3) {
                texture_id = 0; // GL_TEXTURE0
            }
            mesh->set_texture_index(texture_id);
            //mesh2->set_texture_index(texture_id);
            //mesh3->set_texture_index(texture_id);
            //mesh4->set_texture_index(texture_id);
            //mesh5->set_texture_index(texture_id);
            //mesh6->set_texture_index(texture_id);
            break;
        case 'w': // wireframe
            wireframe_mode = !wireframe_mode;
            if(wireframe_mode) {
                glPolygonMode(GL_FRONT, GL_LINE);
                mesh->set_ambient_color(glm::vec3(1));
                hidden_mesh4->set_ambient_color(glm::vec3(1));
            } else {
                glPolygonMode(GL_FRONT, GL_FILL);
                mesh->set_ambient_color(glm::vec3(0));
                hidden_mesh4->set_ambient_color(glm::vec3(0));
            }
            break;
        case 'x': // axis
            show_axis = !show_axis;
            break;
        case 'z': // axis labels
            show_axis_labels = !show_axis_labels;
            break;
        case 32: // space
            do_animation = !do_animation;
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
            light->set_enabled(!light->is_enabled());
            break;
        case GLUT_KEY_F2:
            light2->set_enabled(!light2->is_enabled());
            break;
        case GLUT_KEY_F3:
            light3->set_enabled(!light3->is_enabled());
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
            prev_euler = euler;
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
        mouse_drag = glm::vec2(x, y) - prev_mouse_coord;
    }
    if(left_mouse_down) {
        euler = prev_euler + glm::vec3(0, mouse_drag.y * EULER_PITCH(orbit_speed), mouse_drag.x * EULER_YAW(orbit_speed));
        camera->orbit(euler, orbit_radius);
    }
    if(right_mouse_down) {
        if(camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_PERSPECTIVE) {
            orbit_radius = prev_orbit_radius + mouse_drag.y * dolly_speed;
            camera->orbit(euler, orbit_radius);
        } else if (camera->get_projection_mode() == vt::Camera::PROJECTION_MODE_ORTHO) {
            zoom = prev_zoom + mouse_drag.y * ortho_dolly_speed;
            camera->set_zoom(&zoom);
        }
    }
}

void onReshape(int width, int height)
{
    camera->resize(0, 0, width, height);
    glViewport(0, 0, width, height);
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
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

    const char* s = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
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
        deinit_resources();
    }

    return 0;
}
