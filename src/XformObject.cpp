#include <XformObject.h>
#include <glm/glm.hpp>
#include <set>

namespace vt {

XformObject::XformObject(glm::vec3 origin, glm::vec3 orient, glm::vec3 scale)
    : m_origin(origin),
      m_orient(orient),
      m_scale(scale),
      m_parent(NULL),
      m_is_dirty_xform(true),
      m_is_dirty_normal_xform(true)
{
}

XformObject::~XformObject()
{
}

const glm::mat4 &XformObject::get_xform(bool trace_down)
{
    if(trace_down) {
        update_xform_hier();
        return m_xform;
    }
    if(m_is_dirty_xform) {
        update_xform();
        if(!is_root()) {
            m_xform = m_parent->get_xform(false) * m_xform;
        }
        m_is_dirty_xform = false;
    }
    return m_xform;
}

const glm::mat4 &XformObject::get_normal_xform(bool trace_down)
{
    if(trace_down) {
        update_normal_xform_hier();
        return m_normal_xform;
    }
    if(m_is_dirty_normal_xform) {
        update_normal_xform();
        if(!is_root()) {
            m_normal_xform = m_parent->get_normal_xform(false) * m_normal_xform;
        }
        m_is_dirty_normal_xform = false;
    }
    return m_normal_xform;
}

void XformObject::link_parent(XformObject* parent)
{
    if(parent) {
        parent->get_children().insert(this);
        // TODO: review following
        //imprint();
        //xform_vertices(glm::inverse(parent->get_xform()));
    } else if (!is_root()) {
        std::set<XformObject*>::iterator p = m_parent->get_children().find(this);
        if(p != m_parent->get_children().end()) {
            m_parent->get_children().erase(p);
        }
    }
    m_parent = parent;
}

void XformObject::unlink_children()
{
    // cache container contents in case link_parent alters container being iterated
    std::set<XformObject*> cached_container;
    for(std::set<XformObject*>::iterator p = m_children.begin(); p != m_children.end(); p++) {
        cached_container.insert(*p);
    }
    for(std::set<XformObject*>::iterator q = cached_container.begin(); q != cached_container.end(); q++) {
        (*q)->link_parent(NULL);
    }
}

void XformObject::update_xform_hier()
{
    for(std::set<XformObject*>::iterator p = m_children.begin(); p != m_children.end(); p++) {
        (*p)->mark_dirty_xform(); // mark entire subtree dirty
        (*p)->update_xform_hier();
    }
    if(is_leaf()) {
        get_xform(false); // update entire leaf-root lineage for all leaf nodes
    }
}

void XformObject::update_normal_xform_hier()
{
    for(std::set<XformObject*>::iterator p = m_children.begin(); p != m_children.end(); p++) {
        (*p)->mark_dirty_xform(); // mark entire subtree dirty
        (*p)->update_normal_xform_hier();
    }
    if(is_leaf()) {
        get_normal_xform(false); // update entire leaf-root lineage for all leaf nodes
    }
}

void XformObject::update_normal_xform()
{
    m_normal_xform = glm::transpose(glm::inverse(get_xform()));
}

}
