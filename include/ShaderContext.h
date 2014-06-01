#ifndef VT_SHADER_CONTEXT_H_
#define VT_SHADER_CONTEXT_H_

#include <VarAttribute.h>
#include <VarUniform.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory> // std::unique_ptr

namespace vt {

class Buffer;
class Material;
class Program;
class Texture;

class ShaderContext
{
public:
    typedef std::vector<Texture*> textures_t;

    ShaderContext(
            Material* material,
            Buffer*   vbo_vert_coords,
            Buffer*   vbo_vert_normal,
            Buffer*   vbo_vert_tangent,
            Buffer*   vbo_tex_coords,
            Buffer*   ibo_tri_indices);
    Program *get_program() const
    {
        return m_program;
    }
    void render();
    void set_mvp_xform(glm::mat4 mvp_xform);
    void set_modelview_xform(glm::mat4 modelview_xform);
    void set_normal_xform(glm::mat4 normal_xform);
    void set_texture_index(GLint texture_id);
    void set_normal_map_texture_index(GLint texture_id);
    void set_camera_pos(GLfloat* camera_pos_arr);
    void set_light_pos(size_t num_lights, GLfloat* light_pos_arr);
    void set_light_color(size_t num_lights, GLfloat* light_color_arr);
    void set_light_enabled(size_t num_lights, GLint* light_enabled_arr);
    void set_light_count(GLint light_count);
    void set_env_map_texture_index(GLint texture_id);
    void set_inv_projection_xform(glm::mat4 inv_projection_xform);
    void set_inv_normal_xform(glm::mat4 inv_normal_xform);
    void set_reflect_to_refract_ratio(GLfloat reflect_to_refract_ratio);

private:
    Program* m_program;
    Buffer *m_vbo_vert_coords, *m_vbo_vert_normal, *m_vbo_vert_tangent, *m_vbo_tex_coords, *m_ibo_tri_indices;
    std::unique_ptr<VarAttribute> m_var_attribute_coord3d, m_var_attribute_norm3d, m_var_attribute_tangent3d, m_var_attribute_texcoord;
    std::unique_ptr<VarUniform>
            m_var_uniform_mvp_xform,
            m_var_uniform_modelview_xform,
            m_var_uniform_normal_xform,
            m_var_uniform_mytexture,
            m_var_uniform_normal_map_texture,
            m_var_uniform_camera_pos,
            m_var_uniform_light_pos,
            m_var_uniform_light_color,
            m_var_uniform_light_enabled,
            m_var_uniform_light_count,
            m_var_uniform_env_map_texture,
            m_var_uniform_inv_projection_xform,
            m_var_uniform_inv_normal_xform,
            m_var_uniform_reflect_to_refract_ratio;
    const textures_t &m_textures;
    bool m_use_phong_shading;
    bool m_use_texture_mapping;
    bool m_use_normal_mapping;
    bool m_use_env_mapping;
    bool m_skybox;
};

}

#endif
