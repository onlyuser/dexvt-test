// This file is part of dexvt-lite.
// -- 3D Inverse Kinematics (Cyclic Coordinate Descent) with Constraints
// Copyright (C) 2018 onlyuser <mailto:onlyuser@gmail.com>
//
// dexvt-lite is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// dexvt-lite is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with dexvt-lite.  If not, see <http://www.gnu.org/licenses/>.

#ifndef VT_BBOX_OBJECT_H_
#define VT_BBOX_OBJECT_H_

#include <glm/glm.hpp>

namespace vt {

class BBoxObject
{
public:
    enum align_t {
        ALIGN_CENTER,
        ALIGN_X_MIN,
        ALIGN_X_MAX,
        ALIGN_Y_MIN,
        ALIGN_Y_MAX,
        ALIGN_Z_MIN,
        ALIGN_Z_MAX
    };

    void set_min_max(glm::vec3 min, glm::vec3 max);
    void get_min_max(glm::vec3* min, glm::vec3* max) const;
    glm::vec3 get_center(align_t align = ALIGN_CENTER) const;

protected:
    glm::vec3 m_min;
    glm::vec3 m_max;

    BBoxObject();
};

}

#endif
