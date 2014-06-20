#ifndef VT_MESH_H_
#define VT_MESH_H_

#include <ShaderContext.h>
#include <Buffer.h>
#include <XformObject.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stddef.h>
#include <memory> // std::unique_ptr

namespace vt {

class Material;

class Mesh : public XformObject
{
public:
    Mesh(size_t num_vertex = 0, size_t num_tri = 0);
    virtual ~Mesh();

    size_t get_num_vertex() const {
        return m_num_vertex;
    }

    size_t get_num_tri() const {
        return m_num_tri;
    }

    void set_origin(glm::vec3 origin);
    void set_orient(glm::vec3 orient);

    const bool get_visible() const
    {
        return m_visible;
    }
    void set_visible(bool visible)
    {
        m_visible = visible;
    }

    glm::vec3  get_vert_coord(int index);
    void       set_vert_coord(int index, glm::vec3 coord);
    glm::vec3  get_vert_normal(int index);
    void       set_vert_normal(int index, glm::vec3 normal);
    glm::vec3  get_vert_tangent(int index);
    void       set_vert_tangent(int index, glm::vec3 tangent);
    glm::vec2  get_tex_coord(int index);
    void       set_tex_coord(int index, glm::vec2 coord);
    glm::uvec3 get_tri_indices(int index);
    void       set_tri_indices(int index, glm::uvec3 indices);

    void init_buffers();
    Buffer* get_vbo_vert_coords();
    Buffer* get_vbo_vert_normal();
    Buffer* get_vbo_vert_tangent();
    Buffer* get_vbo_tex_coords();
    Buffer* get_ibo_tri_indices();

    void set_material(Material* material)
    {
        m_material = material;
    }
    Material* get_material() const
    {
        return m_material;
    }

    ShaderContext* get_shader_context();

    int get_texture_index() const
    {
        return m_texture_index;
    }
    void set_texture_index(int texture_index)
    {
        m_texture_index = texture_index;
    }

    int get_normal_map_texture_index() const
    {
        return m_normal_map_texture_index;
    }
    void set_normal_map_texture_index(int normal_map_texture_index)
    {
        m_normal_map_texture_index = normal_map_texture_index;
    }

    int get_env_map_texture_index() const
    {
        return m_env_map_texture_index;
    }
    void set_env_map_texture_index(int env_map_texture_index)
    {
        m_env_map_texture_index = env_map_texture_index;
    }

    int get_depth_map_texture_index() const
    {
        return m_depth_map_texture_index;
    }
    void set_depth_map_texture_index(int depth_map_texture_index)
    {
        m_depth_map_texture_index = depth_map_texture_index;
    }

    float get_reflect_to_refract_ratio() const
    {
        return m_reflect_to_refract_ratio;
    }
    void set_reflect_to_refract_ratio(float reflect_to_refract_ratio)
    {
        m_reflect_to_refract_ratio = reflect_to_refract_ratio;
    }

private:
    size_t                         m_num_vertex;
    size_t                         m_num_tri;
    bool                           m_visible;
    GLfloat*                       m_vert_coords;
    GLfloat*                       m_vert_normal;
    GLfloat*                       m_vert_tangent;
    GLfloat*                       m_tex_coords;
    GLushort*                      m_tri_indices;
    std::unique_ptr<Buffer>        m_vbo_vert_coords;
    std::unique_ptr<Buffer>        m_vbo_vert_normal;
    std::unique_ptr<Buffer>        m_vbo_vert_tangent;
    std::unique_ptr<Buffer>        m_vbo_tex_coords;
    std::unique_ptr<Buffer>        m_ibo_tri_indices;
    bool                           m_buffers_already_init;
    Material*                      m_material; // TODO: Mesh has one Material
    std::unique_ptr<ShaderContext> m_shader_context; // TODO: Mesh has one ShaderContext
    bool                           m_shader_context_already_init;
    int                            m_texture_index;
    int                            m_normal_map_texture_index;
    int                            m_env_map_texture_index;
    int                            m_depth_map_texture_index;
    float                          m_reflect_to_refract_ratio;

    void update_xform();
};

}

#endif
