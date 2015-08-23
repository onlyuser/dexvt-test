#ifndef VT_BBOX_OBJECT_H_
#define VT_BBOX_OBJECT_H_

#include <glm/glm.hpp>

namespace vt {

class BBoxObject
{
public:
    void set_bbox(glm::vec3 min, glm::vec3 max);
    void get_bbox(glm::vec3* min, glm::vec3* max);

protected:
    glm::vec3 m_min;
    glm::vec3 m_max;

    BBoxObject();
};

}

#endif
