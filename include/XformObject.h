#ifndef VT_XFORM_OBJECT_H_
#define VT_XFORM_OBJECT_H_

#include <glm/glm.hpp>
#include <set>

namespace vt {

class XformObject
{
public:
    // for guide wires
    glm::vec3 m_target_dir;
    glm::vec3 m_end_effector_tip_dir;
    glm::vec3 m_local_pivot;
    glm::vec3 m_local_target;

    XformObject(glm::vec3 origin = glm::vec3(0),
                glm::vec3 orient = glm::vec3(0),
                glm::vec3 scale  = glm::vec3(1));
    virtual ~XformObject();

    const glm::vec3 &get_origin() const
    {
        return m_origin;
    }
    void set_origin(glm::vec3 origin);

    const glm::vec3 &get_orient() const
    {
        return m_orient;
    }
    void set_orient(glm::vec3 orient);

    const glm::vec3 get_abs_coord(glm::vec3 local_point = glm::vec3(0));
    const glm::vec3 get_offset_from_origin_in_parent_system(glm::vec3 abs_point);
    const glm::vec3 get_abs_left_direction();
    const glm::vec3 get_abs_up_direction();
    const glm::vec3 get_abs_heading();

    const glm::vec3 &get_scale() const
    {
        return m_scale;
    }
    void set_scale(glm::vec3 scale);

    XformObject* get_parent() const
    {
        return m_parent;
    }
    std::set<XformObject*> &get_children()
    {
        return m_children;
    }
    void reset_xform();
    void link_parent(XformObject* parent, bool keep_xform = false);
    void unlink_children();

    void point_at(glm::vec3 target);
    void rotate(float angle_delta, glm::vec3 pivot);
    bool solve_ik_ccd(
        XformObject* root,
        glm::vec3    local_end_effector_tip,
        glm::vec3    target,
        int          iters,
        float        accept_distance);

    const glm::mat4 &get_xform(bool trace_down = true);
    const glm::mat4 &get_normal_xform();
    void update_xform_hier();

protected:
    glm::vec3 m_origin;
    glm::vec3 m_orient;
    glm::vec3 m_scale;
    glm::mat4 m_xform;
    glm::mat4 m_normal_xform;
    XformObject* m_parent;
    std::set<XformObject*> m_children;

    void mark_dirty_xform()
    {
        m_is_dirty_xform        = true;
        m_is_dirty_normal_xform = true;
    }
    virtual void update_xform() = 0;
    virtual void update_normal_xform();
    glm::mat4 get_local_rotate_xform() const;

private:
    bool m_is_dirty_xform;
    bool m_is_dirty_normal_xform;

    virtual void xform_vertices(glm::mat4 xform) {}
    virtual void rebase(glm::mat4* basis = NULL) {}
    virtual void set_axis(glm::vec3 axis) {}
};

}

#endif
