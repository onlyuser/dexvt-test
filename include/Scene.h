#ifndef VT_SCENE_H_
#define VT_SCENE_H_

#include <GL/glew.h>
#include <vector>
#include <map>
#include <string>

namespace vt {

class Camera;
class Light;
class Material;
class Mesh;
class Texture;

class Scene
{
public:
    typedef std::vector<Light*>    lights_t;
    typedef std::vector<Mesh*>     meshes_t;
    typedef std::vector<Material*> materials_t;
    typedef std::vector<Texture*>  textures_t;

    static Scene* instance()
    {
        static Scene scene;
        return &scene;
    }

    void set_camera(Camera* camera)
    {
        m_camera = camera;
    }
    Camera* get_camera() const
    {
        return m_camera;
    }

    void add_light(Light* light)
    {
        m_lights.push_back(light);
    }

    void add_mesh(Mesh* mesh)
    {
        m_meshes.push_back(mesh);
    }

    void add_material(Material* material)
    {
        m_materials.push_back(material);
    }

    void add_texture(Texture* texture);

    void set_skybox(Mesh* skybox)
    {
        m_skybox = skybox;
    }
    Mesh* get_skybox() const
    {
        return m_skybox;
    }

    void reset();
    void use_program();
    void render();
    void render_vert_normals();

private:
    Camera*     m_camera;
    Mesh*       m_skybox;
    lights_t    m_lights;
    meshes_t    m_meshes;
    materials_t m_materials;
    textures_t  m_textures;

    GLfloat  m_camera_pos[3];
    GLfloat  m_viewport_size[2];
    GLfloat* m_light_pos;
    GLfloat* m_light_color;
    GLint*   m_light_enabled;

    Scene();
    ~Scene();
};

}

#endif
