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

#include <Octree.h>
#include "memory.h"

namespace vt {

Octree::Octree(glm::vec3 origin,
               glm::vec3 dim,
               int       max_depth,
               int       depth,
               Octree*  parent)
    : m_origin(origin),
      m_dim(dim),
      m_center(origin + dim * 0.5f),
      m_max_depth(max_depth),
      m_depth(depth),
      m_parent(parent)
{
    if(depth + 1 == max_depth) {
        memset(m_nodes, 0, sizeof(m_nodes));
        return;
    }
    glm::vec3 node_dim = dim * 0.5f;
    for(int x_index = 0; x_index < 2; x_index++) {
        for(int y_index = 0; y_index < 2; y_index++) {
            for(int z_index = 0; z_index < 2; z_index++) {
                glm::vec3 node_origin = origin + glm::vec3(x_index * node_dim.x,
                                                           y_index * node_dim.y,
                                                           z_index * node_dim.z);
                m_nodes[x_index * 4 + y_index * 2 + z_index] = new Octree(node_origin,
                                                                          node_dim,
                                                                          max_depth,
                                                                          depth + 1,
                                                                          this);
            }
        }
    }
}

Octree::~Octree()
{
    for(int i = 0; i < 8; i++) {
        delete m_nodes[i];
    }
}

bool Octree::encloses(glm::vec3 pos) const
{
    glm::vec3 min = m_origin;
    glm::vec3 max = m_origin + m_dim;
    return pos.x >= min.x && pos.x < max.x &&
           pos.y >= min.y && pos.y < max.y &&
           pos.z >= min.z && pos.z < max.z;
}

const Octree* Octree::downtrace_leaf_enclosing(glm::vec3 pos) const
{
    if(m_depth + 1 == m_max_depth) {
        return this;
    }
    int x_index = pos.x > m_center.x;
    int y_index = pos.y > m_center.y;
    int z_index = pos.z > m_center.z;
    return m_nodes[x_index * 4 + y_index * 2 + z_index]->downtrace_leaf_enclosing(pos);
}

Octree* Octree::uptrace_parent_enclosing(glm::vec3 pos) const
{
    return NULL;
}

bool Octree::add_object(long id, glm::vec3* pos)
{
    std::map<long, glm::vec3*>::iterator p = m_objects.find(id);
    if(p != m_objects.end()) {
        return false;
    }
    m_objects[id] = pos;
    return true;
}

int Octree::find_k_nearest(int k, glm::vec3 pos, std::vector<long>* k_nearest) const
{
    return 0;
}

void Octree::Octree::update()
{
}

}
