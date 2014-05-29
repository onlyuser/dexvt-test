#ifndef VT_CAMERA_H_
#define VT_CAMERA_H_

#include <XformObject.h>
#include <glm/glm.hpp>

namespace vt {

class Camera : public XformObject
{
public:
    enum projection_mode_t {
        PROJECTION_MODE_PERSPECTIVE,
        PROJECTION_MODE_ORTHO
    };

    Camera(
            glm::vec3         origin,
            glm::vec3         target          = glm::vec3(0),
            float             fov             = 45,
            float             width           = 800,
            float             height          = 600,
            float             near_plane      = 0.1,
            float             far_plane       = 100,
            float             ortho_width     = 1,
            float             ortho_height    = 1,
            float             zoom            = 1,
            projection_mode_t projection_mode = PROJECTION_MODE_PERSPECTIVE);
    virtual ~Camera();

    void set_origin(glm::vec3 origin);
    void set_orient(glm::vec3 orient);

    const glm::vec3 get_target() const
    {
        return m_target;
    }
    void set_target(glm::vec3 target);

    void move(glm::vec3 origin, glm::vec3 target = glm::vec3(0));
    void orbit(glm::vec3 &orient, float &radius);

    float get_fov() const
    {
        return m_fov;
    }
    void set_fov(float fov);

    float get_width() const
    {
        return m_width;
    }
    float get_height() const
    {
        return m_height;
    }
    void resize_viewport(float width, float height);

    float get_near_plane() const
    {
        return m_near_plane;
    }
    void set_near_plane(float near_plane);

    float get_far_plane() const
    {
        return m_far_plane;
    }
    void set_far_plane(float far_plane);

    projection_mode_t get_projection_mode() const
    {
        return m_projection_mode;
    }
    void set_projection_mode(projection_mode_t projection_mode);

    float get_ortho_width() const
    {
        return m_ortho_width;
    }
    float get_ortho_height() const
    {
        return m_ortho_height;
    }
    void resize_ortho_viewport(float width, float height);

    float get_zoom() const
    {
        return m_zoom;
    }
    void set_zoom(float &zoom);

    const glm::mat4 &get_view_xform();
    const glm::mat4 &get_projection_xform();

private:
    glm::vec3         m_target;
    float             m_fov;
    size_t            m_width;
    size_t            m_height;
    float             m_near_plane;
    float             m_far_plane;
    glm::mat4         m_view_xform;
    glm::mat4         m_projection_xform;
    bool              m_projection_xform_need_update;
    float             m_ortho_width;
    float             m_ortho_height;
    float             m_zoom;
    projection_mode_t m_projection_mode;

    void update_xform();
    void update_projection_xform();
};

}

#endif
