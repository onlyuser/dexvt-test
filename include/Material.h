#ifndef VT_MATERIAL_H_
#define VT_MATERIAL_H_

#include <NamedObject.h>
#include <Program.h>
#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <string>
#include <memory> // std::unique_ptr

namespace vt {

class Texture;

class Material : public NamedObject
{
public:
    typedef std::vector<Texture*> textures_t;

    Material(
            std::string name                 = "",
            std::string vertex_shader_file   = "",
            std::string fragment_shader_file = "",
            bool        use_ambient_color    = false,
            bool        use_normal_only      = false,
            bool        use_phong_shading    = false,
            bool        use_texture_mapping  = false,
            bool        use_normal_mapping   = false,
            bool        use_env_mapping      = false,
            bool        use_depth_overlay    = false,
            bool        use_ssao             = false,
            bool        use_bloom_kernel     = false,
            bool        use_texture2         = false,
            bool        skybox               = false,
            bool        overlay              = false);
    Program* get_program() const
    {
        return m_program.get();
    }

    glm::vec3 get_ambient_color() const
    {
        return m_ambient_color;
    }
    void add_texture(Texture* texture);
    void clear_textures();
    const textures_t &get_textures() const
    {
        return m_textures;
    }

    bool use_ambient_color() const
    {
        return m_use_ambient_color;
    }
    bool use_normal_only() const
    {
        return m_use_normal_only;
    }
    bool use_phong_shading() const
    {
        return m_use_phong_shading;
    }
    bool use_texture_mapping() const
    {
        return m_use_texture_mapping;
    }
    bool use_normal_mapping() const
    {
        return m_use_normal_mapping;
    }
    bool use_env_mapping() const
    {
        return m_use_env_mapping;
    }
    bool use_depth_overlay() const
    {
        return m_use_depth_overlay;
    }
    bool use_ssao() const
    {
        return m_use_ssao;
    }
    bool use_bloom_kernel() const
    {
        return m_use_bloom_kernel;
    }
    bool use_texture2() const
    {
        return m_use_texture2;
    }
    bool skybox() const
    {
        return m_skybox;
    }
    bool overlay() const
    {
        return m_overlay;
    }

    Texture* get_texture_by_index(int index) const;
    Texture* get_texture_by_name(std::string name) const;
    int get_texture_index_by_name(std::string name) const;

private:
    std::unique_ptr<Program> m_program;
    glm::vec3                m_ambient_color;
    textures_t               m_textures; // TODO: Material has multiple Textures
    bool                     m_use_ambient_color;
    bool                     m_use_normal_only;
    bool                     m_use_phong_shading;
    bool                     m_use_texture_mapping;
    bool                     m_use_normal_mapping;
    bool                     m_use_env_mapping;
    bool                     m_use_depth_overlay;
    bool                     m_use_ssao;
    bool                     m_use_bloom_kernel;
    bool                     m_use_texture2;
    bool                     m_skybox;
    bool                     m_overlay;

    typedef std::map<std::string, Texture*> texture_lookup_table_t;
    texture_lookup_table_t m_texture_lookup_table;
};

}

#endif
