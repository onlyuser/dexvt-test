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

#include <BBoxObject.h>
#include <glm/glm.hpp>

namespace vt {

BBoxObject::BBoxObject()
{
}

void BBoxObject::set_min_max(glm::vec3 min, glm::vec3 max)
{
    m_min = min;
    m_max = max;
}

void BBoxObject::get_min_max(glm::vec3* min, glm::vec3* max) const
{
    if(!min || !max) {
        return;
    }
    *min = m_min;
    *max = m_max;
}

glm::vec3 BBoxObject::get_center(align_t align) const
{
    glm::vec3 center = (m_min + m_max) * 0.5f;
    if(align == ALIGN_CENTER) {
        return center;
    }
    glm::vec3 half_dim = (m_max - m_min) * 0.5f;
    switch(align) {
        case ALIGN_X_MIN:
            center.x -= half_dim.x;
            break;
        case ALIGN_X_MAX:
            center.x += half_dim.x;
            break;
        case ALIGN_Y_MIN:
            center.y -= half_dim.y;
            break;
        case ALIGN_Y_MAX:
            center.y += half_dim.y;
            break;
        case ALIGN_Z_MIN:
            center.z -= half_dim.z;
            break;
        case ALIGN_Z_MAX:
            center.z += half_dim.z;
            break;
        default:
            break;
    }
    return center;
}

}
