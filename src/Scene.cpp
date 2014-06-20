#include <Scene.h>
#include <ShaderContext.h>
#include <Camera.h>
#include <Light.h>
#include <Mesh.h>
#include <Material.h>
#include <Texture.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>
#include <map>
#include <algorithm>
#include <iterator>

#define NUM_LIGHTS 8

namespace vt {

Scene::Scene()
    : m_camera(NULL),
      m_skybox(NULL)
{
    m_camera_pos[0] = 0;
    m_camera_pos[1] = 0;
    m_camera_pos[2] = 0;
    m_light_pos     = new GLfloat[NUM_LIGHTS*3];
    m_light_color   = new GLfloat[NUM_LIGHTS*3];
    m_light_enabled = new GLint[NUM_LIGHTS];
    for(int i = 0; i<NUM_LIGHTS; i++) {
        m_light_pos[i*3+0]   = 0;
        m_light_pos[i*3+1]   = 0;
        m_light_pos[i*3+2]   = 0;
        m_light_color[i*3+0] = 0;
        m_light_color[i*3+1] = 0;
        m_light_color[i*3+2] = 0;
        m_light_enabled[i]   = 0;
    }
}

Scene::~Scene()
{
    if(m_camera) {
        delete m_camera;
    }
    lights_t::const_iterator p;
    for(p = m_lights.begin(); p != m_lights.end(); p++) {
        delete *p;
    }
    meshes_t::const_iterator q;
    for(q = m_meshes.begin(); q != m_meshes.end(); q++) {
        delete *q;
    }
    materials_t::const_iterator r;
    for(r = m_materials.begin(); r != m_materials.end(); r++) {
        delete *r;
    }
    textures_t::const_iterator s;
    for(s = m_textures.begin(); s != m_textures.end(); s++) {
        delete *s;
    }
    if(m_light_pos) {
        delete []m_light_pos;
    }
    if(m_light_color) {
        delete []m_light_color;
    }
    if(m_light_enabled) {
        delete []m_light_enabled;
    }
}

void Scene::add_texture(Texture* texture)
{
    m_textures.push_back(texture);
}

void Scene::reset()
{
    m_camera = NULL;
    m_lights.clear();
    m_meshes.clear();
    m_materials.clear();
    m_textures.clear();
}

void Scene::use_program()
{
    for(meshes_t::const_iterator q = m_meshes.begin(); q != m_meshes.end(); q++) {
        (*q)->get_shader_context()->get_program()->use();
    }
}

void Scene::render()
{
    glm::vec3 camera_pos = m_camera->get_origin();
    m_camera_pos[0] = camera_pos.x;
    m_camera_pos[1] = camera_pos.y;
    m_camera_pos[2] = camera_pos.z;
    int i = 0;
    for(lights_t::const_iterator p = m_lights.begin(); p != m_lights.end(); p++) {
        glm::vec3 light_pos = (*p)->get_origin();
        m_light_pos[i*3+0] = light_pos.x;
        m_light_pos[i*3+1] = light_pos.y;
        m_light_pos[i*3+2] = light_pos.z;
        glm::vec3 light_color = (*p)->get_color();
        m_light_color[i*3+0] = light_color.r;
        m_light_color[i*3+1] = light_color.g;
        m_light_color[i*3+2] = light_color.b;
        m_light_enabled[i] = (*p)->get_enabled();
        i++;
    }
    if(m_skybox) {
        ShaderContext* shader_context = m_skybox->get_shader_context();
        shader_context->get_program()->use();
        shader_context->set_env_map_texture_index(0);
        shader_context->set_inv_normal_xform(glm::inverse(m_camera->get_normal_xform()));
        shader_context->set_inv_projection_xform(glm::inverse(m_camera->get_projection_xform()));
        shader_context->render();
    }
    for(meshes_t::const_iterator q = m_meshes.begin(); q != m_meshes.end(); q++) {
        if(!(*q)->get_visible()) {
            continue;
        }
        Material*      material       = (*q)->get_material();
        ShaderContext* shader_context = (*q)->get_shader_context();
        shader_context->get_program()->use();
        shader_context->set_mvp_xform(m_camera->get_xform()*(*q)->get_xform());
        if(material->use_phong_shading() ||
                material->use_normal_mapping() ||
                material->use_env_mapping())
        {
            shader_context->set_modelview_xform((*q)->get_xform());
            shader_context->set_normal_xform((*q)->get_normal_xform());
            shader_context->set_camera_pos(m_camera_pos);
            if(material->use_phong_shading()) {
                shader_context->set_light_pos(NUM_LIGHTS, m_light_pos);
                shader_context->set_light_color(NUM_LIGHTS, m_light_color);
                shader_context->set_light_enabled(NUM_LIGHTS, m_light_enabled);
                shader_context->set_light_count(m_lights.size());
            }
            if(material->use_normal_mapping()) {
                shader_context->set_normal_map_texture_index((*q)->get_normal_map_texture_index());
            }
            if(material->use_env_mapping()) {
                shader_context->set_env_map_texture_index(0);
                shader_context->set_reflect_to_refract_ratio((*q)->get_reflect_to_refract_ratio());
            }
            if(material->use_depth_map()) {
                shader_context->set_depth_map_texture_index((*q)->get_depth_map_texture_index());
            }
        }
        if(material->use_texture_mapping()) {
            shader_context->set_texture_index((*q)->get_texture_index());
        }
        shader_context->render();
    }
}

void Scene::render_vert_normals()
{
    const float axis_surface_distance = 0.05;
    const float axis_arm_length       = 0.1;
    const float light_radius          = 0.125;

    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf((const GLfloat*) &m_camera->get_projection_xform());
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glColor3f(1, 1, 0);
    for(lights_t::const_iterator p = m_lights.begin(); p != m_lights.end(); p++) {
        glm::mat4 modelview_xform = m_camera->get_view_xform()*(*p)->get_xform();
        glLoadMatrixf((const GLfloat*) &modelview_xform[0]);
        glutWireSphere(light_radius, 4, 2);
    }
    for(meshes_t::const_iterator q = m_meshes.begin(); q != m_meshes.end(); q++) {
        if(!(*q)->get_visible()) {
            continue;
        }
        glm::mat4 modelview_xform = m_camera->get_view_xform()*(*q)->get_xform();
        glLoadMatrixf((const GLfloat*) &modelview_xform[0]);
        glColor3f(0, 0, 1);
        glBegin(GL_LINES);
        for (int i=0; i<static_cast<int>((*q)->get_num_vertex()); i++){
            glm::vec3 v = (*q)->get_vert_coord(i);
            v += (*q)->get_vert_normal(i)*axis_surface_distance;
            glVertex3fv(&v.x);
            v += (*q)->get_vert_normal(i)*axis_arm_length;
            glVertex3fv(&v.x);
        }
        glEnd();
        glColor3f(1, 0, 0);
        glBegin(GL_LINES);
        for (int i=0; i<static_cast<int>((*q)->get_num_vertex()); i++){
            glm::vec3 v = (*q)->get_vert_coord(i);
            v += (*q)->get_vert_normal(i)*axis_surface_distance;
            glVertex3fv(&v.x);
            v += (*q)->get_vert_tangent(i)*axis_arm_length;
            glVertex3fv(&v.x);
        }
        glEnd();
        glColor3f(0, 1, 0);
        glBegin(GL_LINES);
        for (int i=0; i<static_cast<int>((*q)->get_num_vertex()); i++){
            glm::vec3 v = (*q)->get_vert_coord(i);
            v += (*q)->get_vert_normal(i)*axis_surface_distance;
            glVertex3fv(&v.x);
            glm::vec3 bitangent = glm::normalize(glm::cross((*q)->get_vert_normal(i), (*q)->get_vert_tangent(i)));
            v += bitangent*axis_arm_length;
            glVertex3fv(&v.x);
        }
        glEnd();
    }
    glPopMatrix();
}

}
