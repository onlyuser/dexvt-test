#include <BBoxObject.h>

namespace vt {

BBoxObject::BBoxObject()
{
}

void BBoxObject::set_bbox(glm::vec3 min, glm::vec3 max)
{
    m_min = min;
    m_max = max;
}

void BBoxObject::get_bbox(glm::vec3* min, glm::vec3* max)
{
    if(!min || max) {
        return;
    }
    *min = m_min;
    *max = m_max;
}

}
