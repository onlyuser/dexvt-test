#include <Light.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vt {

Light::Light(glm::vec3 origin, glm::vec3 color)
    : XformObject(origin),
      m_color(color),
      m_enabled(true)
{
}

void Light::update_xform()
{
    m_xform = glm::translate(glm::mat4(1), m_origin);
}

}
