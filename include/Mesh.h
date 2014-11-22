#ifndef VT_MESH_H_
#define VT_MESH_H_

#include <NamedObject.h>
#include <ShaderContext.h>
#include <Buffer.h>
#include <XformObject.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <stddef.h>
#include <memory> // std::unique_ptr

namespace vt {

class Material;

class Mesh : public NamedObject, public XformObject
{
public:
    Mesh(
            std::string name       = "",
            size_t      num_vertex = 0,
            size_t      num_tri    = 0);
    virtual ~Mesh();

    size_t get_num_vertex() const
    {
        return m_num_vertex;
    }

    size_t get_num_tri() const
    {
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
    void update_buffers() const;
    Buffer* get_vbo_vert_coords();
    Buffer* get_vbo_vert_normal();
    Buffer* get_vbo_vert_tangent();
    Buffer* get_vbo_tex_coords();
    Buffer* get_ibo_tri_indices();

    void set_material(Material* material);
    Material* get_material() const
    {
        return m_material;
    }

    ShaderContext* get_shader_context();
    ShaderContext* get_normal_shader_context(Material* normal_material);
    ShaderContext* get_wireframe_shader_context(Material* wireframe_material);
    ShaderContext* get_ssao_shader_context(Material* ssao_material);

    int get_texture_slot_index() const
    {
        return m_texture_id;
    }
    void set_texture_id(int texture_id)
    {
        m_texture_id = texture_id;
    }

    int get_texture2_index() const
    {
        return m_texture2_index;
    }
    void set_texture2_index(int texture_id)
    {
        m_texture2_index = texture_id;
    }

    int get_bump_texture_id() const
    {
        return m_bump_texture_id;
    }
    void set_bump_texture_id(int bump_texture_id)
    {
        m_bump_texture_id = bump_texture_id;
    }

    int get_env_map_texture_id() const
    {
        return m_env_map_texture_id;
    }
    void set_env_map_texture_id(int env_map_texture_id)
    {
        m_env_map_texture_id = env_map_texture_id;
    }

    int get_random_texture_id() const
    {
        return m_random_texture_id;
    }
    void set_random_texture_id(int random_texture_id)
    {
        m_random_texture_id = random_texture_id;
    }

    int get_frontface_depth_overlay_texture_id() const
    {
        return m_frontface_depth_overlay_texture_id;
    }
    void set_frontface_depth_overlay_texture_id(int frontface_depth_overlay_texture_id)
    {
        m_frontface_depth_overlay_texture_id = frontface_depth_overlay_texture_id;
    }

    int get_backface_depth_overlay_texture_id() const
    {
        return m_backface_depth_overlay_texture_id;
    }
    void set_backface_depth_overlay_texture_id(int backface_depth_overlay_texture_id)
    {
        m_backface_depth_overlay_texture_id = backface_depth_overlay_texture_id;
    }

    int get_backface_normal_overlay_texture_id() const
    {
        return m_backface_normal_overlay_texture_id;
    }
    void set_backface_normal_overlay_texture_id(int backface_normal_overlay_texture_id)
    {
        m_backface_normal_overlay_texture_id = backface_normal_overlay_texture_id;
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
    std::string                    m_name;
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
    Material*                      m_material;                 // TODO: Mesh has one Material
    std::unique_ptr<ShaderContext> m_shader_context;           // TODO: Mesh has one ShaderContext
    std::unique_ptr<ShaderContext> m_normal_shader_context;    // TODO: Mesh has one normal ShaderContext
    std::unique_ptr<ShaderContext> m_wireframe_shader_context; // TODO: Mesh has one wireframe ShaderContext
    std::unique_ptr<ShaderContext> m_ssao_shader_context;      // TODO: Mesh has one ssao ShaderContext
    int                            m_texture_id;
    int                            m_texture2_index;
    int                            m_bump_texture_id;
    int                            m_env_map_texture_id;
    int                            m_random_texture_id;
    int                            m_frontface_depth_overlay_texture_id;
    int                            m_backface_depth_overlay_texture_id;
    int                            m_backface_normal_overlay_texture_id;
    float                          m_reflect_to_refract_ratio;

    void update_xform();
};

}

#endif
