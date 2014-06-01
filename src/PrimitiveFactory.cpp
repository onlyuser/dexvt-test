#include <PrimitiveFactory.h>
#include <Mesh.h>
#include <Util.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace vt {

Mesh* PrimitiveFactory::create_grid(int cols, int rows, float width, float height)
{
    int num_vertex = (rows+1)*(cols+1);
    int num_tri = rows*cols*2;
    Mesh* mesh = new Mesh(num_vertex, num_tri);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row_pos = 0; row_pos <= rows; row_pos++) {
        for(int col_pos = 0; col_pos <= cols; col_pos++) {
            mesh->set_vert_coord(vert_index, glm::vec3(
                    width*(static_cast<float>(col_pos)/cols),
                    height*(static_cast<float>(row_pos)/rows),
                    0));
            mesh->set_vert_normal( vert_index, glm::vec3(0, 0, 1));
            mesh->set_vert_tangent(vert_index, glm::vec3(1, 0, 0));
            vert_index++;
        }
    }

    // ========================
    // init mesh texture coords
    // ========================

    int tex_vert_index = 0;
    for(int row_pos = 0; row_pos <= rows; row_pos++) {
        for(int col_pos = 0; col_pos <= cols; col_pos++) {
            mesh->set_tex_coord(tex_vert_index++, glm::vec2(
                    static_cast<float>(col_pos)/cols,
                    static_cast<float>(row_pos)/rows));
        }
    }

    // ==========================
    // init mesh triangle indices
    // ==========================

    int tri_index = 0;
    for(int row_pos = 0; row_pos<rows; row_pos++) {
        for(int col_pos = 0; col_pos<cols; col_pos++) {
            int lower_left  = row_pos*(cols+1)+col_pos;
            int lower_right = lower_left+1;
            int upper_left  = (row_pos+1)*(cols+1)+col_pos;
            int upper_right = upper_left+1;
            mesh->set_tri_indices(tri_index++, glm::uvec3(lower_left, lower_right, upper_right));
            mesh->set_tri_indices(tri_index++, glm::uvec3(upper_right, upper_left, lower_left));
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_sphere(int slices, int stacks, float radius)
{
    int cols = slices;
    int rows = stacks;
    Mesh* mesh = create_grid(cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row_pos = 0; row_pos <= rows; row_pos++) {
        for(int col_pos = 0; col_pos <= cols; col_pos++) {
            glm::vec3 normal = orient_to_offset(glm::vec3(
                    0,
                    -(static_cast<float>(row_pos)/rows*180-90), // pitch
                    static_cast<float>(col_pos)/cols*360));     // yaw
            glm::vec3 offset = normal*radius;
            mesh->set_vert_coord(vert_index, offset);
            mesh->set_vert_normal(vert_index, glm::normalize(offset));
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                          // pitch
                    static_cast<float>(col_pos)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_hemisphere(int slices, int stacks, float radius)
{
    int cols = slices;
    int rows = stacks*0.5+1;
    Mesh* mesh = create_grid(cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row_pos = 0; row_pos <= rows; row_pos++) {
        for(int col_pos = 0; col_pos <= cols; col_pos++) {
            if(row_pos == 0) {
                mesh->set_vert_coord(vert_index, glm::vec3(0, 0, 0));
                mesh->set_vert_normal(vert_index, glm::vec3(0, -1, 0));
            } else {
                glm::vec3 offset = orient_to_offset(glm::vec3(
                        0,
                        -(static_cast<float>(row_pos-1)/(rows-1)*90), // pitch
                        static_cast<float>(col_pos)/cols*360))        // yaw
                        *radius;
                mesh->set_vert_coord(vert_index, offset);
                mesh->set_vert_normal(vert_index, glm::normalize(offset));
            }
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                          // pitch
                    static_cast<float>(col_pos)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_cylinder(int slices, float radius, float height)
{
    int cols = slices;
    int rows = 3;
    Mesh* mesh = create_grid(cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row_pos = 0; row_pos <= rows; row_pos++) {
        for(int col_pos = 0; col_pos <= cols; col_pos++) {
            if(row_pos == 0) {
                mesh->set_vert_coord(vert_index, glm::vec3(0, 0, 0));
                mesh->set_vert_normal(vert_index, glm::vec3(0, -1, 0));
            } else if(row_pos == 1) {
                glm::vec3 offset = orient_to_offset(glm::vec3(
                        0,
                        0,                                     // pitch
                        static_cast<float>(col_pos)/cols*360)) // yaw
                        *radius;
                mesh->set_vert_coord(vert_index, glm::vec3(offset.x, 0, offset.z));
                mesh->set_vert_normal(vert_index, glm::normalize(offset));
            }
            if(row_pos == 3) {
                mesh->set_vert_coord(vert_index, glm::vec3(0, height, 0));
                mesh->set_vert_normal(vert_index, glm::vec3(0, 1, 0));
            } else if(row_pos == 2) {
                glm::vec3 offset = orient_to_offset(glm::vec3(
                        0,
                        0,                                     // pitch
                        static_cast<float>(col_pos)/cols*360)) // yaw
                        *radius;
                mesh->set_vert_coord(vert_index, glm::vec3(offset.x, height, offset.z));
                mesh->set_vert_normal(vert_index, glm::normalize(offset));
            }
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                          // pitch
                    static_cast<float>(col_pos)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_cone(int slices, float radius, float height)
{
    int cols = slices;
    int rows = 2;
    Mesh* mesh = create_grid(cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    float rim_y_offset = radius*sin(glm::half_pi<float>()-atan(height/radius));
    int vert_index = 0;
    for(int row_pos = 0; row_pos <= rows; row_pos++) {
        for(int col_pos = 0; col_pos <= cols; col_pos++) {
            if(row_pos == 0) {
                mesh->set_vert_coord(vert_index, glm::vec3(0, 0, 0));
                mesh->set_vert_normal(vert_index, glm::vec3(0, -1, 0));
            } else if(row_pos == 1) {
                glm::vec3 offset = orient_to_offset(glm::vec3(
                        0,
                        0,                                     // pitch
                        static_cast<float>(col_pos)/cols*360)) // yaw
                        *radius;
                mesh->set_vert_coord(vert_index, offset);
                mesh->set_vert_normal(vert_index, glm::normalize(offset+glm::vec3(0, rim_y_offset, 0)));
            }
            if(row_pos == 2) {
                glm::vec3 offset = orient_to_offset(glm::vec3(
                        0,
                        0,                                     // pitch
                        static_cast<float>(col_pos)/cols*360)) // yaw
                        *radius;
                mesh->set_vert_coord(vert_index, glm::vec3(0, height, 0));
                mesh->set_vert_normal(vert_index, glm::normalize(offset+glm::vec3(0, rim_y_offset, 0)));
            }
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                          // pitch
                    static_cast<float>(col_pos)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_torus(int slices, int stacks, float radius_major, float radius_minor)
{
    int cols = slices;
    int rows = stacks;
    Mesh* mesh = create_grid(cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row_pos = 0; row_pos <= rows; row_pos++) {
        for(int col_pos = 0; col_pos <= cols; col_pos++) {
            glm::vec3 normal_major = orient_to_offset(glm::vec3(
                    0,
                    0,                                      // pitch
                    static_cast<float>(col_pos)/cols*360)); // yaw
            glm::vec3 normal_minor = orient_to_offset(glm::vec3(
                    0,
                    -(static_cast<float>(row_pos)/rows*360-180), // pitch
                    static_cast<float>(col_pos)/cols*360));      // yaw
            mesh->set_vert_coord(vert_index, normal_major*radius_major+normal_minor*radius_minor);
            mesh->set_vert_normal(vert_index, normal_minor);
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                          // pitch
                    static_cast<float>(col_pos)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_box(float width, float height, float length)
{
    Mesh* mesh = new Mesh(24, 12);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    // right
    mesh->set_vert_coord(0, glm::vec3(0, 0,      0));
    mesh->set_vert_coord(1, glm::vec3(0, 0,      length));
    mesh->set_vert_coord(2, glm::vec3(0, height, length));
    mesh->set_vert_coord(3, glm::vec3(0, height, 0));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 0*4+i, glm::vec3(-1, 0, 0));
        mesh->set_vert_tangent(0*4+i, glm::vec3( 0, 0, 1));
    }

    // front
    mesh->set_vert_coord(4, glm::vec3(0,     0,      length));
    mesh->set_vert_coord(5, glm::vec3(width, 0,      length));
    mesh->set_vert_coord(6, glm::vec3(width, height, length));
    mesh->set_vert_coord(7, glm::vec3(0,     height, length));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 1*4+i, glm::vec3(0, 0, 1));
        mesh->set_vert_tangent(1*4+i, glm::vec3(1, 0, 0));
    }

    // left
    mesh->set_vert_coord( 8, glm::vec3(width, 0,      length));
    mesh->set_vert_coord( 9, glm::vec3(width, 0,      0));
    mesh->set_vert_coord(10, glm::vec3(width, height, 0));
    mesh->set_vert_coord(11, glm::vec3(width, height, length));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 2*4+i, glm::vec3(1, 0,  0));
        mesh->set_vert_tangent(2*4+i, glm::vec3(0, 0, -1));
    }

    // back
    mesh->set_vert_coord(12, glm::vec3(width, 0,      0));
    mesh->set_vert_coord(13, glm::vec3(0,     0,      0));
    mesh->set_vert_coord(14, glm::vec3(0,     height, 0));
    mesh->set_vert_coord(15, glm::vec3(width, height, 0));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 3*4+i, glm::vec3( 0, 0, -1));
        mesh->set_vert_tangent(3*4+i, glm::vec3(-1, 0,  0));
    }

    // top
    mesh->set_vert_coord(16, glm::vec3(width, height, 0));
    mesh->set_vert_coord(17, glm::vec3(0,     height, 0));
    mesh->set_vert_coord(18, glm::vec3(0,     height, length));
    mesh->set_vert_coord(19, glm::vec3(width, height, length));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 4*4+i, glm::vec3( 0, 1, 0));
        mesh->set_vert_tangent(4*4+i, glm::vec3(-1, 0, 0));
    }

    // bottom
    mesh->set_vert_coord(20, glm::vec3(0,     0, 0));
    mesh->set_vert_coord(21, glm::vec3(width, 0, 0));
    mesh->set_vert_coord(22, glm::vec3(width, 0, length));
    mesh->set_vert_coord(23, glm::vec3(0,     0, length));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 5*4+i, glm::vec3(0, -1, 0));
        mesh->set_vert_tangent(5*4+i, glm::vec3(1,  0, 0));
    }

    // ========================
    // init mesh texture coords
    // ========================

    // right
    mesh->set_tex_coord(0, glm::vec2(0, 0));
    mesh->set_tex_coord(1, glm::vec2(1, 0));
    mesh->set_tex_coord(2, glm::vec2(1, 1));
    mesh->set_tex_coord(3, glm::vec2(0, 1));

    for(int i=1; i<6; i++) {
        mesh->set_tex_coord(i*4+0, mesh->get_tex_coord(0));
        mesh->set_tex_coord(i*4+1, mesh->get_tex_coord(1));
        mesh->set_tex_coord(i*4+2, mesh->get_tex_coord(2));
        mesh->set_tex_coord(i*4+3, mesh->get_tex_coord(3));
    }

    // ==========================
    // init mesh triangle indices
    // ==========================

    // right
    mesh->set_tri_indices(0, glm::uvec3(0, 1, 2));
    mesh->set_tri_indices(1, glm::uvec3(2, 3, 0));

    // front
    mesh->set_tri_indices(2, glm::uvec3(4, 5, 6));
    mesh->set_tri_indices(3, glm::uvec3(6, 7, 4));

    // left
    mesh->set_tri_indices(4, glm::uvec3(8,  9, 10));
    mesh->set_tri_indices(5, glm::uvec3(10, 11, 8));

    // back
    mesh->set_tri_indices(6, glm::uvec3(12, 13, 14));
    mesh->set_tri_indices(7, glm::uvec3(14, 15, 12));

    // top
    mesh->set_tri_indices(8, glm::uvec3(16, 17, 18));
    mesh->set_tri_indices(9, glm::uvec3(18, 19, 16));

    // bottom
    mesh->set_tri_indices(10, glm::uvec3(20, 21, 22));
    mesh->set_tri_indices(11, glm::uvec3(22, 23, 20));

    return mesh;
}

}
