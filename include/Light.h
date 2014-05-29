#ifndef VT_LIGHT_H_
#define VT_LIGHT_H_

#include <XformObject.h>
#include <glm/glm.hpp>

namespace vt {

class Light : public XformObject
{
public:
    Light(glm::vec3 origin, glm::vec3 color);

    const glm::vec3 get_color() const
    {
        return m_color;
    }

    const bool get_enabled() const
    {
        return m_enabled;
    }
    void set_enabled(bool enabled)
    {
        m_enabled = enabled;
    }

private:
    glm::vec3 m_color;
    bool      m_enabled;

    void update_xform();
};

}

#endif
