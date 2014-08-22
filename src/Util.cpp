#include <Util.h>
#include <Mesh.h>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/constants.hpp>
#include <math.h>

#define ROLL(v)  v.x
#define PITCH(v) v.y
#define YAW(v)   v.z

#define EPSILON 0.0001

#define SIGN(x) (!(x) ? 0 : (((x)>0) ? 1 : -1))

namespace vt {

glm::vec3 orient_to_offset(glm::vec3 orient)
{
    static glm::vec3 left    = glm::vec3(1, 0, 0);
    static glm::vec3 up      = glm::vec3(0, 1, 0);
    static glm::vec3 forward = glm::vec3(0, 0, 1);
    glm::mat4 pitch = glm::rotate(
            glm::mat4(1),
            PITCH(orient), left);
    glm::mat4 yaw = glm::rotate(
            glm::mat4(1),
            YAW(orient), up);
    return glm::vec3(yaw*pitch*glm::vec4(forward, 1));
}

glm::vec3 offset_to_orient(glm::vec3 offset)
{
    static glm::vec3 forward = glm::vec3(0, 0, 1);
    glm::vec3 t(offset.x, 0, offset.z); // flattened offset
    glm::vec3 r(
        0,
        glm::angle(t, offset),
        glm::angle(t, forward));
    if(static_cast<float>(fabs(offset.x))<EPSILON && static_cast<float>(fabs(offset.z))<EPSILON)
    {
        PITCH(r) = -SIGN(offset.y)*glm::radians(90.0f);
        YAW(r) = 0; // undefined
        return r;
    }
    if(offset.x < 0) YAW(r) *= -1;
    if(offset.y > 0) PITCH(r) *= -1;
    return r;
}

void mesh_calc_normals_and_tangents(Mesh* mesh)
{
    for(int i=0; i<static_cast<int>(mesh->get_num_tri()); i++)
    {
        glm::uvec3 tri_indices = mesh->get_tri_indices(i);
        glm::vec3 p0 = mesh->get_vert_coord(tri_indices[0]);
        glm::vec3 p1 = mesh->get_vert_coord(tri_indices[1]);
        glm::vec3 p2 = mesh->get_vert_coord(tri_indices[2]);
        glm::vec3 e1 = glm::normalize(p1 - p0);
        glm::vec3 e2 = glm::normalize(p2 - p0);
        glm::vec3 n = glm::normalize(glm::cross(e1, e2));
        for(int j=0; j<3; j++)
        {
            mesh->set_vert_normal( tri_indices[j], n);
            mesh->set_vert_tangent(tri_indices[j], e1);
        }
    }
}

void mesh_apply_ripple(Mesh* mesh, glm::vec3 origin, float amplitude, float wavelength, float phase)
{
    for(int i=0; i<static_cast<int>(mesh->get_num_vertex()); i++)
    {
        glm::vec3 pos = mesh->get_vert_coord(i);
        glm::vec3 new_pos = pos;
        new_pos.y = origin.y + static_cast<float>(sin(glm::distance(glm::vec2(origin.x, origin.z), glm::vec2(pos.x, pos.z))/(wavelength/(glm::pi<float>()*2)) + phase))*amplitude;
        mesh->set_vert_coord(i, new_pos);
    }

    mesh_calc_normals_and_tangents(mesh);
}

}
