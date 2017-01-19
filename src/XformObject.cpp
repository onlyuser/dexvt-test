#include <XformObject.h>
#include <Util.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/glm.hpp>
#include <set>

namespace vt {

XformObject::XformObject(
        glm::vec3 origin,
        glm::vec3 orient,
        glm::vec3 scale)
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

void XformObject::set_origin(glm::vec3 origin)
{
    m_origin = origin;
    mark_dirty_xform();
}

void XformObject::set_orient(glm::vec3 orient)
{
    m_orient = orient;
    mark_dirty_xform();
}

void XformObject::set_scale(glm::vec3 scale)
{
    m_scale = scale;
    mark_dirty_xform();
}

void XformObject::reset_xform()
{
    m_origin = glm::vec3(0);
    m_orient = glm::vec3(0);
    m_scale  = glm::vec3(1);
    mark_dirty_xform();
}

const glm::vec3 XformObject::map_to_abs_coord(glm::vec3 local_point)
{
    return glm::vec3(get_xform() * glm::vec4(local_point, 1));
}

const glm::vec3 XformObject::map_to_parent_coord(glm::vec3 abs_point)
{
    if(m_parent) {
        return glm::vec3(glm::inverse(m_parent->get_xform()) * glm::vec4(abs_point, 1));
    }
    return abs_point;
}

const glm::vec3 XformObject::map_to_origin_in_parent_coord(glm::vec3 abs_point)
{
    return map_to_parent_coord(abs_point) - m_origin;
}

const glm::vec3 XformObject::get_abs_left_direction()
{
    return glm::vec3(get_normal_xform() * glm::vec4(VEC_LEFT, 1));
}

const glm::vec3 XformObject::get_abs_up_direction()
{
    return glm::vec3(get_normal_xform() * glm::vec4(VEC_UP, 1));
}

const glm::vec3 XformObject::get_abs_heading()
{
    return glm::vec3(get_normal_xform() * glm::vec4(VEC_FORWARD, 1));
}

void XformObject::link_parent(XformObject* parent, bool keep_xform)
{
    glm::vec3 abs_origin;
    if(keep_xform) {
        abs_origin = map_to_abs_coord();
    }
    if(parent) {
        if(keep_xform) {
            // unproject to global space and reproject to parent space
            glm::mat4 parent_inverse_xform = glm::inverse(parent->get_xform());
            rebase(&parent_inverse_xform);

            // break all connections -- TODO: review this
            link_parent(NULL);
            unlink_children();
        }

        // make new parent remember you
        parent->get_children().insert(this);
    } else {
        if(keep_xform) {
            // unproject to global space
            rebase();

            //link_parent(NULL); // infinite recursion
            unlink_children();
        }

        if(m_parent) {
            // make parent disown you
            std::set<XformObject*>::iterator p = m_parent->get_children().find(this);
            if(p != m_parent->get_children().end()) {
                m_parent->get_children().erase(p);
            }
        }
    }
    m_parent = parent;
    if(keep_xform) {
        set_axis(abs_origin);
    } else {
        reset_xform();
    }
}

void XformObject::unlink_children()
{
    for(std::set<XformObject*>::iterator p = m_children.begin(); p != m_children.end(); p++) {
        (*p)->link_parent(NULL);
    }
}

void XformObject::point_at(glm::vec3 target)
{
    set_orient(offset_to_orient(map_to_origin_in_parent_coord(target)));
}

void XformObject::rotate(float angle_delta, glm::vec3 pivot)
{
    glm::mat4 rotate_xform                  = GLM_ROTATE(glm::mat4(1), angle_delta, pivot);
    glm::vec3 abs_origin                    = map_to_abs_coord();
    glm::vec3 parent_local_new_heading      = map_to_origin_in_parent_coord(abs_origin + glm::vec3(rotate_xform * glm::vec4(get_abs_heading(), 1)));
    glm::vec3 parent_local_new_up_direction = map_to_origin_in_parent_coord(abs_origin + glm::vec3(rotate_xform * glm::vec4(get_abs_up_direction(), 1)));
    set_orient(offset_to_orient(parent_local_new_heading, &parent_local_new_up_direction));
}

// http://what-when-how.com/advanced-methods-in-computer-graphics/kinematics-advanced-methods-in-computer-graphics-part-4/
bool XformObject::solve_ik_ccd(XformObject* root,
                               glm::vec3    local_end_effector_tip,
                               glm::vec3    target,
                               int          iters,
                               float        accept_distance)
{
    for(int i = 0; i < iters; i++) {
        //int index = 0;
        for(XformObject* current_segment = this; current_segment && current_segment != root->get_parent(); current_segment = current_segment->get_parent())
        {
            glm::vec3 end_effector_tip = map_to_abs_coord(local_end_effector_tip);
            if(glm::distance(end_effector_tip, target) < accept_distance) {
                return true;
            }
#if 1
            // attempt #4 -- same as attempt #3, but make use of roll component for each segment
            glm::vec3 local_target_dir                 = glm::normalize(current_segment->map_to_origin_in_parent_coord(target));
            glm::vec3 local_end_effector_tip_dir       = glm::normalize(current_segment->map_to_origin_in_parent_coord(end_effector_tip));
            glm::vec3 local_arc_dir                    = glm::normalize(local_target_dir - local_end_effector_tip_dir);
            glm::vec3 local_arc_midpoint_dir           = glm::normalize((local_target_dir + local_end_effector_tip_dir) * 0.5f);
            glm::vec3 local_arc_pivot                  = glm::cross(local_arc_dir, local_arc_midpoint_dir);
            float     angle_delta                      = glm::degrees(glm::angle(local_target_dir, local_end_effector_tip_dir));
            glm::mat4 current_segment_rotate_xform     = current_segment->get_local_rotate_xform();
            glm::mat4 local_arc_rotate_xform           = GLM_ROTATE(glm::mat4(1), -angle_delta, local_arc_pivot);
            glm::vec3 new_current_segment_heading      = glm::vec3(local_arc_rotate_xform * current_segment_rotate_xform * glm::vec4(VEC_FORWARD, 1));
            glm::vec3 new_current_segment_up_direction = glm::vec3(local_arc_rotate_xform * current_segment_rotate_xform * glm::vec4(VEC_UP, 1));
            if(i < iters - 1) { // reserve last iter for updating guide wires
                current_segment->set_orient(offset_to_orient(new_current_segment_heading, &new_current_segment_up_direction));
            }
            current_segment->m_debug_target_dir           = local_target_dir;
            current_segment->m_debug_end_effector_tip_dir = local_end_effector_tip_dir;
            current_segment->m_debug_local_pivot          = local_arc_pivot;
            current_segment->m_debug_local_target         = current_segment->map_to_origin_in_parent_coord(target);
#if 0
            std::cout << "INDEX: " << index << std::endl;
            std::cout << "TARGET: " << glm::to_string(local_target_dir) << ", END_EFF: " << glm::to_string(local_end_effector_tip_dir) << ", ANGLE: " << angle_delta << std::endl;
            std::cout << "BEFORE: " << glm::to_string(current_segment_heading) << ", AFTER: " << glm::to_string(new_current_segment_heading) << std::endl;
            std::cout << "PIVOT: " << glm::to_string(local_arc_pivot) << std::endl;
#endif
#elif 1
            // attempt #3 -- do rotations in Cartesian coordinates
            glm::vec3 local_target_dir            = glm::normalize(current_segment->map_to_origin_in_parent_coord(target));
            glm::vec3 local_end_effector_tip_dir  = glm::normalize(current_segment->map_to_origin_in_parent_coord(end_effector_tip));
            glm::vec3 local_arc_dir               = glm::normalize(local_target_dir - local_end_effector_tip_dir);
            glm::vec3 local_arc_midpoint_dir      = glm::normalize((local_target_dir + local_end_effector_tip_dir) * 0.5f);
            glm::vec3 local_arc_pivot             = glm::cross(local_arc_dir, local_arc_midpoint_dir);
            float     angle_delta                 = glm::degrees(glm::angle(local_target_dir, local_end_effector_tip_dir));
            glm::vec3 current_segment_heading     = orient_to_offset(current_segment->get_orient());
            glm::vec3 new_current_segment_heading = glm::vec3(GLM_ROTATE(glm::mat4(1), -angle_delta, local_arc_pivot) * glm::vec4(current_segment_heading, 1));
            current_segment->set_orient(offset_to_orient(new_current_segment_heading));
#elif 1
            // attempt #2 -- do rotations in Euler coordinates with special handling for angle loop-around
            glm::vec3 local_target_orient           = offset_to_orient(current_segment->map_to_origin_in_parent_coord(target));
            glm::vec3 local_end_effector_tip_orient = offset_to_orient(current_segment->map_to_origin_in_parent_coord(end_effector_tip));
            glm::vec3 orient_delta                  = orient_diff(local_target_orient, local_end_effector_tip_orient);
            current_segment->set_orient(orient_sum(current_segment->get_orient(), orient_delta));
#else
            // attempt #1 -- do rotations in Euler coordinates
            glm::vec3 local_target_orient           = offset_to_orient(current_segment->map_to_origin_in_parent_coord(target));
            glm::vec3 local_end_effector_tip_orient = offset_to_orient(current_segment->map_to_origin_in_parent_coord(end_effector_tip));
            glm::vec3 orient_delta                  = local_target_orient - local_end_effector_tip_orient;
            current_segment->set_orient(current_segment->get_orient() + orient_delta);
#endif
            //index++;
            //return true;
        }
    }
    return false;
}

const glm::mat4 &XformObject::get_xform(bool trace_down)
{
    if(trace_down) {
        update_xform_hier();
        return m_xform;
    }
    if(m_is_dirty_xform) {
        update_xform();
        if(m_parent) {
            m_xform = m_parent->get_xform(false) * m_xform;
        }
        m_is_dirty_xform = false;
    }
    return m_xform;
}

const glm::mat4 &XformObject::get_normal_xform()
{
    if(m_is_dirty_normal_xform) {
        update_normal_xform();
        m_is_dirty_normal_xform = false;
    }
    return m_normal_xform;
}

glm::mat4 XformObject::get_local_rotate_xform() const
{
    return GLM_ROTATE(glm::mat4(1), static_cast<float>(ORIENT_YAW(m_orient)),   VEC_UP) *
           GLM_ROTATE(glm::mat4(1), static_cast<float>(ORIENT_PITCH(m_orient)), VEC_LEFT) *
           GLM_ROTATE(glm::mat4(1), static_cast<float>(ORIENT_ROLL(m_orient)),  VEC_FORWARD);
}

void XformObject::update_xform_hier()
{
    for(std::set<XformObject*>::iterator p = m_children.begin(); p != m_children.end(); p++) {
        (*p)->mark_dirty_xform(); // mark entire subtree dirty
        (*p)->update_xform_hier();
    }
    if(m_children.empty()) {
        get_xform(false); // update entire leaf-to-root lineage for all leaf nodes
    }
}

void XformObject::update_normal_xform()
{
    m_normal_xform = glm::transpose(glm::inverse(get_xform()));
}

}
