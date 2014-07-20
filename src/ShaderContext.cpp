#include <ShaderContext.h>
#include <Buffer.h>
#include <Material.h>
#include <Program.h>
#include <Texture.h>
#include <VarAttribute.h>
#include <VarUniform.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory> // std::unique_ptr

#define NUM_LIGHTS 8

namespace vt {

ShaderContext::ShaderContext(
        Material* material,
        Buffer*   vbo_vert_coords,
        Buffer*   vbo_vert_normal,
        Buffer*   vbo_vert_tangent,
        Buffer*   vbo_tex_coords,
        Buffer*   ibo_tri_indices)
    : m_material(material),
      m_vbo_vert_coords(vbo_vert_coords),
      m_vbo_vert_normal(vbo_vert_normal),
      m_vbo_vert_tangent(vbo_vert_tangent),
      m_vbo_tex_coords(vbo_tex_coords),
      m_ibo_tri_indices(ibo_tri_indices),
      m_textures(material->get_textures()),
      m_use_normal_only(material->use_normal_only()),
      m_use_camera_vec(material->use_camera_vec()),
      m_use_phong_shading(material->use_phong_shading()),
      m_use_texture_mapping(material->use_texture_mapping()),
      m_use_normal_mapping(material->use_normal_mapping()),
      m_use_env_mapping(material->use_env_mapping()),
      m_use_depth_overlay(material->use_depth_overlay()),
      m_skybox(material->skybox())
{
    bool use_phong_normal_env = m_use_phong_shading || m_use_normal_mapping || m_use_env_mapping;
    Program *program = material->get_program();
    if(m_use_normal_only || m_use_camera_vec || use_phong_normal_env) {
        m_var_attribute_vertex_normal = std::unique_ptr<VarAttribute>(program->get_var_attribute("vertex_normal"));
        m_var_uniform_normal_xform    = std::unique_ptr<VarUniform>(program->get_var_uniform("normal_xform"));
        if(m_use_camera_vec || use_phong_normal_env) {
            if(m_use_camera_vec || (!m_use_normal_only && m_use_normal_mapping) || m_use_env_mapping) {
                m_var_uniform_model_xform = std::unique_ptr<VarUniform>(program->get_var_uniform("model_xform"));
                m_var_uniform_camera_pos  = std::unique_ptr<VarUniform>(program->get_var_uniform("camera_position"));
            }
            if(m_use_phong_shading) {
                m_var_uniform_light_pos     = std::unique_ptr<VarUniform>(program->get_var_uniform("light_position"));
                m_var_uniform_light_color   = std::unique_ptr<VarUniform>(program->get_var_uniform("light_color"));
                m_var_uniform_light_enabled = std::unique_ptr<VarUniform>(program->get_var_uniform("light_enabled"));
                m_var_uniform_light_count   = std::unique_ptr<VarUniform>(program->get_var_uniform("light_count"));
            }
            if(m_use_normal_mapping) {
                m_var_attribute_vertex_tangent   = std::unique_ptr<VarAttribute>(program->get_var_attribute("vertex_tangent"));
                m_var_uniform_normal_map_texture = std::unique_ptr<VarUniform>(program->get_var_uniform("normal_map_texture"));
            }
            if(m_use_env_mapping) {
                m_var_uniform_env_map_texture          = std::unique_ptr<VarUniform>(program->get_var_uniform("env_map_texture"));
                m_var_uniform_reflect_to_refract_ratio = std::unique_ptr<VarUniform>(program->get_var_uniform("reflect_to_refract_ratio"));
            }
        }
    }
    if(m_use_texture_mapping || m_use_normal_mapping) {
        m_var_attribute_texcoord = std::unique_ptr<VarAttribute>(program->get_var_attribute("texcoord"));
        if(m_use_texture_mapping) {
            m_var_uniform_color_texture = std::unique_ptr<VarUniform>(program->get_var_uniform("color_texture"));
        }
    }
    if(m_use_depth_overlay) {
        m_var_uniform_front_depth_overlay_texture = std::unique_ptr<VarUniform>(program->get_var_uniform("front_depth_overlay_texture"));
        m_var_uniform_back_depth_overlay_texture  = std::unique_ptr<VarUniform>(program->get_var_uniform("back_depth_overlay_texture"));
        m_var_uniform_back_normal_overlay_texture = std::unique_ptr<VarUniform>(program->get_var_uniform("back_normal_overlay_texture"));
        m_var_uniform_viewport_dim                = std::unique_ptr<VarUniform>(program->get_var_uniform("viewport_dim"));
        m_var_uniform_camera_near                 = std::unique_ptr<VarUniform>(program->get_var_uniform("camera_near"));
        m_var_uniform_camera_far                  = std::unique_ptr<VarUniform>(program->get_var_uniform("camera_far"));
    }
    if(m_skybox) {
        m_var_uniform_env_map_texture      = std::unique_ptr<VarUniform>(program->get_var_uniform("env_map_texture"));
        m_var_uniform_inv_projection_xform = std::unique_ptr<VarUniform>(program->get_var_uniform("inv_projection_xform"));
        m_var_uniform_inv_normal_xform     = std::unique_ptr<VarUniform>(program->get_var_uniform("inv_normal_xform"));
    } else {
        m_var_attribute_vertex_position = std::unique_ptr<VarAttribute>(program->get_var_attribute("vertex_position"));
        m_var_uniform_mvp_xform         = std::unique_ptr<VarUniform>(program->get_var_uniform("mvp_xform"));
    }
}

void ShaderContext::render()
{
    m_material->get_program()->use();
    int i = 0;
    for(ShaderContext::textures_t::const_iterator p = m_textures.begin(); p != m_textures.end(); p++)
    {
        glActiveTexture(GL_TEXTURE0+i);
        (*p)->bind();
        i++;
    }
    if(m_skybox) {
        glDisable(GL_DEPTH_TEST);
        glBegin(GL_QUADS);
            glVertex3f(-1.0, -1.0, 0.0);
            glVertex3f( 1.0, -1.0, 0.0);
            glVertex3f( 1.0,  1.0, 0.0);
            glVertex3f(-1.0,  1.0, 0.0);
        glEnd();
        glEnable(GL_DEPTH_TEST);
        return;
    }
    m_var_attribute_vertex_position->enable_vertex_attrib_array();
    m_var_attribute_vertex_position->vertex_attrib_pointer(
            m_vbo_vert_coords,
            3,        // number of elements per vertex, here (x,y,z)
            GL_FLOAT, // the type of each element
            GL_FALSE, // take our values as-is
            0,        // no extra data between each position
            0);       // offset of first element
    if(m_use_normal_only || m_use_phong_shading || m_use_normal_mapping || m_use_env_mapping) {
        m_var_attribute_vertex_normal->enable_vertex_attrib_array();
        m_var_attribute_vertex_normal->vertex_attrib_pointer(
                m_vbo_vert_normal,
                3,        // number of elements per vertex, here (x,y,z)
                GL_FLOAT, // the type of each element
                GL_FALSE, // take our values as-is
                0,        // no extra data between each position
                0);       // offset of first element
        if(m_use_normal_mapping) {
            m_var_attribute_vertex_tangent->enable_vertex_attrib_array();
            m_var_attribute_vertex_tangent->vertex_attrib_pointer(
                    m_vbo_vert_tangent,
                    3,        // number of elements per vertex, here (x,y,z)
                    GL_FLOAT, // the type of each element
                    GL_FALSE, // take our values as-is
                    0,        // no extra data between each position
                    0);       // offset of first element
        }
    }
    if(m_use_texture_mapping || m_use_normal_mapping) {
        m_var_attribute_texcoord->enable_vertex_attrib_array();
        m_var_attribute_texcoord->vertex_attrib_pointer(
                m_vbo_tex_coords,
                2,        // number of elements per vertex, here (x,y)
                GL_FLOAT, // the type of each element
                GL_FALSE, // take our values as-is
                0,        // no extra data between each position
                0);       // offset of first element
    }
    if(m_ibo_tri_indices) {
        m_ibo_tri_indices->bind();
        glDrawElements(GL_TRIANGLES, m_ibo_tri_indices->size()/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    }
    m_var_attribute_vertex_position->disable_vertex_attrib_array();
    if(m_use_normal_only || m_use_phong_shading || m_use_normal_mapping || m_use_env_mapping) {
        m_var_attribute_vertex_normal->disable_vertex_attrib_array();
        if(m_use_normal_mapping) {
            m_var_attribute_vertex_tangent->disable_vertex_attrib_array();
        }
    }
    if(m_use_texture_mapping || m_use_normal_mapping) {
        m_var_attribute_texcoord->disable_vertex_attrib_array();
    }
}

void ShaderContext::set_mvp_xform(glm::mat4 mvp_xform)
{
    m_var_uniform_mvp_xform->uniform_matrix_4fv(1, GL_FALSE, glm::value_ptr(mvp_xform));
}

void ShaderContext::set_model_xform(glm::mat4 model_xform)
{
    m_var_uniform_model_xform->uniform_matrix_4fv(1, GL_FALSE, glm::value_ptr(model_xform));
}

void ShaderContext::set_normal_xform(glm::mat4 normal_xform)
{
    m_var_uniform_normal_xform->uniform_matrix_4fv(1, GL_FALSE, glm::value_ptr(normal_xform));
}

void ShaderContext::set_texture_index(GLint texture_id)
{
    assert(texture_id >= 0 && texture_id < static_cast<int>(m_textures.size()));
    m_var_uniform_color_texture->uniform_1i(texture_id);
}

void ShaderContext::set_normal_map_texture_index(GLint texture_id)
{
    assert(texture_id >= 0 && texture_id < static_cast<int>(m_textures.size()));
    m_var_uniform_normal_map_texture->uniform_1i(texture_id);
}

void ShaderContext::set_camera_pos(GLfloat* camera_pos_arr)
{
    m_var_uniform_camera_pos->uniform_3fv(1, camera_pos_arr);
}

void ShaderContext::set_light_pos(size_t num_lights, GLfloat* light_pos_arr)
{
    m_var_uniform_light_pos->uniform_3fv(num_lights, light_pos_arr);
}

void ShaderContext::set_light_color(size_t num_lights, GLfloat* light_color_arr)
{
    m_var_uniform_light_color->uniform_3fv(num_lights, light_color_arr);
}

void ShaderContext::set_light_enabled(size_t num_lights, GLint* light_enabled_arr)
{
    m_var_uniform_light_enabled->uniform_1iv(num_lights, light_enabled_arr);
}

void ShaderContext::set_light_count(GLint light_count)
{
    m_var_uniform_light_count->uniform_1i(light_count);
}

void ShaderContext::set_env_map_texture_index(GLint texture_id)
{
    assert(texture_id >= 0 && texture_id < static_cast<int>(m_textures.size()));
    m_var_uniform_env_map_texture->uniform_1i(texture_id);
}

void ShaderContext::set_inv_projection_xform(glm::mat4 inv_projection_xform)
{
    m_var_uniform_inv_projection_xform->uniform_matrix_4fv(1, GL_FALSE, glm::value_ptr(inv_projection_xform));
}

void ShaderContext::set_inv_normal_xform(glm::mat4 inv_normal_xform)
{
    m_var_uniform_inv_normal_xform->uniform_matrix_4fv(1, GL_FALSE, glm::value_ptr(inv_normal_xform));
}

void ShaderContext::set_front_depth_overlay_texture_index(GLint texture_id)
{
    assert(texture_id >= 0);
    if(texture_id < static_cast<int>(m_textures.size())) {
        m_var_uniform_front_depth_overlay_texture->uniform_1i(texture_id);
    }
}

void ShaderContext::set_back_depth_overlay_texture_index(GLint texture_id)
{
    assert(texture_id >= 0 && texture_id < static_cast<int>(m_textures.size()));
    m_var_uniform_back_depth_overlay_texture->uniform_1i(texture_id);
}

void ShaderContext::set_back_normal_overlay_texture_index(GLint texture_id)
{
    assert(texture_id >= 0 && texture_id < static_cast<int>(m_textures.size()));
    m_var_uniform_back_normal_overlay_texture->uniform_1i(texture_id);
}

void ShaderContext::set_viewport_dim(GLfloat* viewport_dim_arr)
{
    m_var_uniform_viewport_dim->uniform_2fv(1, viewport_dim_arr);
}

void ShaderContext::set_camera_near(GLfloat camera_near)
{
    m_var_uniform_camera_near->uniform_1f(camera_near);
}

void ShaderContext::set_camera_far(GLfloat camera_far)
{
    m_var_uniform_camera_far->uniform_1f(camera_far);
}

void ShaderContext::set_reflect_to_refract_ratio(GLfloat reflect_to_refract_ratio)
{
    m_var_uniform_reflect_to_refract_ratio->uniform_1f(reflect_to_refract_ratio);
}

}
