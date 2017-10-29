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

#ifndef VT_OCTREE_H_
#define VT_OCTREE_H_

#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace vt {

class Octree
{
public:
    Octree(glm::vec3 origin,
           glm::vec3 dim,
           int       max_depth,
           int       depth = 0,
           Octree*  parent = NULL);
    virtual ~Octree();
    glm::vec3     get_origin() const        { return m_origin; }
    glm::vec3     get_dim() const           { return m_dim; }
    glm::vec3     get_center() const        { return m_center; }
    int           get_max_depth() const     { return m_max_depth; }
    int           get_depth() const         { return m_depth; }
    Octree*       get_parent() const        { return m_parent; }
    Octree*       get_node(int index) const { return m_nodes[index]; }
    bool          encloses(glm::vec3 pos) const;
    const Octree* downtrace_leaf_enclosing(glm::vec3 pos) const;
    Octree*       uptrace_parent_enclosing(glm::vec3 pos) const;
    bool          add_object(long id, glm::vec3* pos);
    int           find_k_nearest(int k, glm::vec3 pos, std::vector<long>* k_nearest) const;
    void          update();

private:
    glm::vec3                  m_origin;
    glm::vec3                  m_dim;
    glm::vec3                  m_center;
    int                        m_max_depth;
    int                        m_depth;
    Octree*                    m_parent;
    Octree*                    m_nodes[8];
    std::map<long, glm::vec3*> m_objects;
};

}

#endif
