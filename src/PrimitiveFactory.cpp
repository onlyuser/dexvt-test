#include <PrimitiveFactory.h>
#include <Mesh.h>
#include <Util.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/constants.hpp>
#include <string>

namespace vt {

Mesh* PrimitiveFactory::create_grid(
        std::string name,
        int         cols,
        int         rows,
        float       width,
        float       height)
{
    int   num_vertex = (rows+1)*(cols+1);
    int   num_tri    = rows*cols*2;
    Mesh* mesh       = new Mesh(name, num_vertex, num_tri);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row = 0; row <= rows; row++) {
        for(int col = 0; col <= cols; col++) {
            mesh->set_vert_coord(vert_index, glm::vec3(
                    width*(static_cast<float>(col)/cols),
                    height*(static_cast<float>(row)/rows),
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
    for(int row = 0; row <= rows; row++) {
        for(int col = 0; col <= cols; col++) {
            mesh->set_tex_coord(tex_vert_index++, glm::vec2(
                    static_cast<float>(col)/cols,
                    1-static_cast<float>(row)/rows));
        }
    }

    // ==========================
    // init mesh triangle indices
    // ==========================

    int tri_index = 0;
    for(int row = 0; row<rows; row++) {
        for(int col = 0; col<cols; col++) {
            int lower_left  = row*(cols+1)+col;
            int lower_right = lower_left+1;
            int upper_left  = (row+1)*(cols+1)+col;
            int upper_right = upper_left+1;
            mesh->set_tri_indices(tri_index++, glm::uvec3(lower_left, lower_right, upper_right));
            mesh->set_tri_indices(tri_index++, glm::uvec3(upper_right, upper_left, lower_left));
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_sphere(
        std::string name,
        int         slices,
        int         stacks,
        float       radius)
{
    int cols   = slices;
    int rows   = stacks;
    Mesh* mesh = create_grid(name, cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row = 0; row <= rows; row++) {
        for(int col = 0; col <= cols; col++) {
            glm::vec3 normal = orient_to_offset(glm::vec3(
                    0,
                    -(static_cast<float>(row)/rows*180-90), // pitch
                    static_cast<float>(col)/cols*360));     // yaw
            glm::vec3 offset = normal*radius;
            mesh->set_vert_coord(vert_index, offset);
            mesh->set_vert_normal(vert_index, glm::normalize(offset));
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                      // pitch
                    static_cast<float>(col)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_hemisphere(
        std::string name,
        int         slices,
        int         stacks,
        float       radius)
{
    int   cols = slices;
    int   rows = stacks*0.5+2;
    Mesh* mesh = create_grid(name, cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row = 0; row <= rows; row++) {
        for(int col = 0; col <= cols; col++) {
            switch(row) {
                case 0:
                    mesh->set_vert_coord(vert_index, glm::vec3(0, 0, 0));
                    mesh->set_vert_normal(vert_index, glm::vec3(0, -1, 0));
                    break;
                default:
                    {
                        glm::vec3 offset = orient_to_offset(glm::vec3(
                                0,
                                -(static_cast<float>(row-1)/(rows-1)*90), // pitch
                                static_cast<float>(col)/cols*360))        // yaw
                                *radius;
                        mesh->set_vert_coord(vert_index, offset);
                        mesh->set_vert_normal(vert_index, (row == 1) ?
                                glm::vec3(0, -1, 0) : glm::normalize(offset));
                    }
                    break;
            }
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                      // pitch
                    static_cast<float>(col)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_cylinder(
        std::string name,
        int         slices,
        float       radius,
        float       height)
{
    int   cols = slices;
    int   rows = 5;
    Mesh* mesh = create_grid(name, cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row = 0; row <= rows; row++) {
        for(int col = 0; col <= cols; col++) {
            switch(row) {
                case 0:
                    mesh->set_vert_coord(vert_index, glm::vec3(0, 0, 0));
                    mesh->set_vert_normal(vert_index, glm::vec3(0, -1, 0));
                    break;
                case 1:
                case 2:
                    {
                        glm::vec3 offset = orient_to_offset(glm::vec3(
                                0,
                                0,                                 // pitch
                                static_cast<float>(col)/cols*360)) // yaw
                                *radius;
                        mesh->set_vert_coord(vert_index, glm::vec3(offset.x, 0, offset.z));
                        mesh->set_vert_normal(vert_index, (row == 1) ?
                                glm::vec3(0, -1, 0) : glm::normalize(offset));
                    }
                    break;
                case 3:
                case 4:
                    {
                        glm::vec3 offset = orient_to_offset(glm::vec3(
                                0,
                                0,                                 // pitch
                                static_cast<float>(col)/cols*360)) // yaw
                                *radius;
                        mesh->set_vert_coord(vert_index, glm::vec3(offset.x, height, offset.z));
                        mesh->set_vert_normal(vert_index, (row == 4) ?
                                glm::vec3(0, 1, 0) : glm::normalize(offset));
                    }
                    break;
                case 5:
                    mesh->set_vert_coord(vert_index, glm::vec3(0, height, 0));
                    mesh->set_vert_normal(vert_index, glm::vec3(0, 1, 0));
                    break;
            }
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                      // pitch
                    static_cast<float>(col)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_cone(
        std::string name,
        int         slices,
        float       radius,
        float       height)
{
    int   cols = slices;
    int   rows = 3;
    Mesh* mesh = create_grid(name, cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    float rim_y_offset = radius*sin(glm::half_pi<float>()-atan(height/radius));
    int vert_index = 0;
    for(int row = 0; row <= rows; row++) {
        for(int col = 0; col <= cols; col++) {
            switch(row) {
                case 0:
                    mesh->set_vert_coord(vert_index, glm::vec3(0, 0, 0));
                    mesh->set_vert_normal(vert_index, glm::vec3(0, -1, 0));
                    break;
                case 1:
                case 2:
                    {
                        glm::vec3 offset = orient_to_offset(glm::vec3(
                                0,
                                0,                                 // pitch
                                static_cast<float>(col)/cols*360)) // yaw
                                *radius;
                        mesh->set_vert_coord(vert_index, offset);
                        mesh->set_vert_normal(vert_index, (row == 1) ?
                                glm::vec3(0, -1, 0) : glm::normalize(offset+glm::vec3(0, rim_y_offset, 0)));
                    }
                    break;
                case 3:
                    {
                        glm::vec3 offset = orient_to_offset(glm::vec3(
                                0,
                                0,                                 // pitch
                                static_cast<float>(col)/cols*360)) // yaw
                                *radius;
                        mesh->set_vert_coord(vert_index, glm::vec3(0, height, 0));
                        mesh->set_vert_normal(vert_index, glm::normalize(offset+glm::vec3(0, rim_y_offset, 0)));
                    }
                    break;
            }
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                      // pitch
                    static_cast<float>(col)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_torus(
        std::string name,
        int         slices,
        int         stacks,
        float       radius_major,
        float       radius_minor)
{
    int   cols = slices;
    int   rows = stacks;
    Mesh* mesh = create_grid(name, cols, rows);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    int vert_index = 0;
    for(int row = 0; row <= rows; row++) {
        for(int col = 0; col <= cols; col++) {
            glm::vec3 normal_major = orient_to_offset(glm::vec3(
                    0,
                    0,                                  // pitch
                    static_cast<float>(col)/cols*360)); // yaw
            glm::vec3 normal_minor = orient_to_offset(glm::vec3(
                    0,
                    -(static_cast<float>(row)/rows*360-180), // pitch
                    static_cast<float>(col)/cols*360));      // yaw
            mesh->set_vert_coord(vert_index, normal_major*radius_major+normal_minor*radius_minor);
            mesh->set_vert_normal(vert_index, normal_minor);
            mesh->set_vert_tangent(vert_index, orient_to_offset(glm::vec3(
                    0,
                    0,                                      // pitch
                    static_cast<float>(col)/cols*360+90))); // yaw
            vert_index++;
        }
    }

    return mesh;
}

Mesh* PrimitiveFactory::create_box(
        std::string name,
        float       width,
        float       height,
        float       length)
{
    Mesh* mesh = new Mesh(name, 24, 12);

    // ==============================
    // init mesh vertex/normal coords
    // ==============================

    // right
    mesh->set_vert_coord(0, glm::vec3(0, 0, 0));
    mesh->set_vert_coord(1, glm::vec3(0, 0, 1));
    mesh->set_vert_coord(2, glm::vec3(0, 1, 1));
    mesh->set_vert_coord(3, glm::vec3(0, 1, 0));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 0*4+i, glm::vec3(-1, 0, 0));
        mesh->set_vert_tangent(0*4+i, glm::vec3( 0, 0, 1));
    }

    // front
    mesh->set_vert_coord(4, glm::vec3(0, 0, 1));
    mesh->set_vert_coord(5, glm::vec3(1, 0, 1));
    mesh->set_vert_coord(6, glm::vec3(1, 1, 1));
    mesh->set_vert_coord(7, glm::vec3(0, 1, 1));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 1*4+i, glm::vec3(0, 0, 1));
        mesh->set_vert_tangent(1*4+i, glm::vec3(1, 0, 0));
    }

    // left
    mesh->set_vert_coord( 8, glm::vec3(1, 0, 1));
    mesh->set_vert_coord( 9, glm::vec3(1, 0, 0));
    mesh->set_vert_coord(10, glm::vec3(1, 1, 0));
    mesh->set_vert_coord(11, glm::vec3(1, 1, 1));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 2*4+i, glm::vec3(1, 0,  0));
        mesh->set_vert_tangent(2*4+i, glm::vec3(0, 0, -1));
    }

    // back
    mesh->set_vert_coord(12, glm::vec3(1, 0, 0));
    mesh->set_vert_coord(13, glm::vec3(0, 0, 0));
    mesh->set_vert_coord(14, glm::vec3(0, 1, 0));
    mesh->set_vert_coord(15, glm::vec3(1, 1, 0));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 3*4+i, glm::vec3( 0, 0, -1));
        mesh->set_vert_tangent(3*4+i, glm::vec3(-1, 0,  0));
    }

    // top
    mesh->set_vert_coord(16, glm::vec3(1, 1, 0));
    mesh->set_vert_coord(17, glm::vec3(0, 1, 0));
    mesh->set_vert_coord(18, glm::vec3(0, 1, 1));
    mesh->set_vert_coord(19, glm::vec3(1, 1, 1));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 4*4+i, glm::vec3( 0, 1, 0));
        mesh->set_vert_tangent(4*4+i, glm::vec3(-1, 0, 0));
    }

    // bottom
    mesh->set_vert_coord(20, glm::vec3(0, 0, 0));
    mesh->set_vert_coord(21, glm::vec3(1, 0, 0));
    mesh->set_vert_coord(22, glm::vec3(1, 0, 1));
    mesh->set_vert_coord(23, glm::vec3(0, 0, 1));
    for(int i=0; i<4; i++) {
        mesh->set_vert_normal( 5*4+i, glm::vec3(0, -1, 0));
        mesh->set_vert_tangent(5*4+i, glm::vec3(1,  0, 0));
    }

    glm::mat4 scale_xform = glm::scale(glm::mat4(1), glm::vec3(width, height, length));
    for(int i=1; i<static_cast<int>(mesh->get_num_vertex()); i++) {
        mesh->set_vert_coord(i, glm::vec3(glm::vec4(mesh->get_vert_coord(i), 1)*scale_xform));
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

Mesh* PrimitiveFactory::create_tetrahedron(
        std::string name,
        float       width,
        float       height,
        float       length)
{
    Mesh* mesh = new Mesh(name, 12, 4);

    // triangle opposite to origin
    mesh->set_vert_coord(0, glm::vec3(1, 0, 0));
    mesh->set_vert_coord(1, glm::vec3(0, 0, 1));
    mesh->set_vert_coord(2, glm::vec3(0, 1, 0));

    // triangle starting at x
    mesh->set_vert_coord(3, glm::vec3(1, 0, 0));
    mesh->set_vert_coord(4, glm::vec3(0, 1, 0));
    mesh->set_vert_coord(5, glm::vec3(1, 1, 1));

    // triangle starting at y
    mesh->set_vert_coord(6, glm::vec3(0, 1, 0));
    mesh->set_vert_coord(7, glm::vec3(0, 0, 1));
    mesh->set_vert_coord(8, glm::vec3(1, 1, 1));

    // triangle starting at z
    mesh->set_vert_coord(9,  glm::vec3(0, 0, 1));
    mesh->set_vert_coord(10, glm::vec3(1, 0, 0));
    mesh->set_vert_coord(11, glm::vec3(1, 1, 1));

    for(int i=0; i<static_cast<int>(mesh->get_num_tri()); i++) {
        glm::vec3 p0 = mesh->get_vert_coord(i*3+0);
        glm::vec3 p1 = mesh->get_vert_coord(i*3+1);
        glm::vec3 p2 = mesh->get_vert_coord(i*3+2);
        glm::vec3 e1 = glm::normalize(p1-p0);
        glm::vec3 e2 = glm::normalize(p2-p0);
        glm::vec3 n = glm::normalize(glm::cross(e1, e2));
        for(int j=0; j<3; j++) {
            mesh->set_vert_normal( i*3+j, n);
            mesh->set_vert_tangent(i*3+j, e1);
        }
    }

    mesh->set_tri_indices(0, glm::uvec3(0, 1,  2));
    mesh->set_tri_indices(1, glm::uvec3(3, 4,  5));
    mesh->set_tri_indices(2, glm::uvec3(6, 7,  8));
    mesh->set_tri_indices(3, glm::uvec3(9, 10, 11));

    return mesh;
}

Mesh* PrimitiveFactory::create_diamond_brilliant_cut(
        std::string name,
        float       radius,
        float       table_radius,
        float       height,
        float       crown_height_to_total_height_ratio,
        float       upper_girdle_height_to_crown_height_ratio,
        float       lower_girdle_depth_to_pavilion_depth_ratio,
        float       girdle_thick_part_thickness,
        float       girdle_thin_part_thickness)
{
    Mesh* mesh = new Mesh(name, 336, 112);//new Mesh(name, 336, 112);

    float crown_height   = height*crown_height_to_total_height_ratio;
    float pavilion_depth = height-crown_height;

    float upper_girdle_inner_rim_y      = pavilion_depth+crown_height*upper_girdle_height_to_crown_height_ratio;
    float upper_girdle_inner_rim_radius = radius-(radius-table_radius)*upper_girdle_height_to_crown_height_ratio;

    float girdle_thick_part_top_y    = pavilion_depth+girdle_thick_part_thickness*0.5;
    float girdle_thick_part_bottom_y = pavilion_depth-girdle_thick_part_thickness*0.5;
    float girdle_thin_part_top_y     = pavilion_depth+girdle_thin_part_thickness*0.5;
    float girdle_thin_part_bottom_y  = pavilion_depth-girdle_thin_part_thickness*0.5;

    float lower_girdle_inner_rim_y      = pavilion_depth*(1-lower_girdle_depth_to_pavilion_depth_ratio);
    float lower_girdle_inner_rim_radius = radius*lower_girdle_depth_to_pavilion_depth_ratio;

    upper_girdle_inner_rim_radius /= cos(glm::pi<float>()*0.125);
    lower_girdle_inner_rim_radius /= cos(glm::pi<float>()*0.125);

    int vert_index = 0;
    int tri_index  = 0;

    // table: 8 triangles
    for(int i=0; i<8; i++) {
        glm::vec3 p1 = glm::vec3(0, height, 0);
        glm::vec3 p2 = orient_to_offset(glm::vec3(
                0,
                0,                            // pitch
                static_cast<float>(i)/8*360)) // yaw
                *table_radius
                +glm::vec3(0, height, 0);
        glm::vec3 p3 = orient_to_offset(glm::vec3(
                0,
                0,                              // pitch
                static_cast<float>(i+1)/8*360)) // yaw
                *table_radius
                +glm::vec3(0, height, 0);
        mesh->set_vert_coord(vert_index+0, p1);
        mesh->set_vert_coord(vert_index+1, p2);
        mesh->set_vert_coord(vert_index+2, p3);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;
    }

    // star: 8 triangles
    for(int i=0; i<8; i++) {
        glm::vec3 p1 = orient_to_offset(glm::vec3(
                0,
                0,                            // pitch
                static_cast<float>(i)/8*360)) // yaw
                *table_radius
                +glm::vec3(0, height, 0);
        glm::vec3 p2 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *upper_girdle_inner_rim_radius
                +glm::vec3(0, upper_girdle_inner_rim_y, 0);
        glm::vec3 p3 = orient_to_offset(glm::vec3(
                0,
                0,                              // pitch
                static_cast<float>(i+1)/8*360)) // yaw
                *table_radius
                +glm::vec3(0, height, 0);
        mesh->set_vert_coord(vert_index+0, p1);
        mesh->set_vert_coord(vert_index+1, p2);
        mesh->set_vert_coord(vert_index+2, p3);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;
    }

    // kite: 8*2 triangles
    for(int i=0; i<8; i++) {
        // half kite 1
        glm::vec3 p1 = orient_to_offset(glm::vec3(
                0,
                0,                            // pitch
                static_cast<float>(i)/8*360)) // yaw
                *table_radius
                +glm::vec3(0, height, 0);
        glm::vec3 p2 = orient_to_offset(glm::vec3(
                0,
                0,                            // pitch
                static_cast<float>(i)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_top_y, 0);
        glm::vec3 p3 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *upper_girdle_inner_rim_radius
                +glm::vec3(0, upper_girdle_inner_rim_y, 0);
        mesh->set_vert_coord(vert_index+0, p1);
        mesh->set_vert_coord(vert_index+1, p2);
        mesh->set_vert_coord(vert_index+2, p3);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;

        // half kite 2
        glm::vec3 p4 = orient_to_offset(glm::vec3(
                0,
                0,                              // pitch
                static_cast<float>(i+1)/8*360)) // yaw
                *table_radius
                +glm::vec3(0, height, 0);
        glm::vec3 p5 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *upper_girdle_inner_rim_radius
                +glm::vec3(0, upper_girdle_inner_rim_y, 0);
        glm::vec3 p6 = orient_to_offset(glm::vec3(
                0,
                0,                              // pitch
                static_cast<float>(i+1)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_top_y, 0);
        mesh->set_vert_coord(vert_index+0, p4);
        mesh->set_vert_coord(vert_index+1, p5);
        mesh->set_vert_coord(vert_index+2, p6);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;
    }

    // upper-girdle: 16 triangles
    for(int i=0; i<8; i++) {
        // half upper-girdle 1
        glm::vec3 p1 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *upper_girdle_inner_rim_radius
                +glm::vec3(0, upper_girdle_inner_rim_y, 0);
        glm::vec3 p2 = orient_to_offset(glm::vec3(
                0,
                0,                            // pitch
                static_cast<float>(i)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_top_y, 0);
        glm::vec3 p3 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thin_part_top_y, 0);
        mesh->set_vert_coord(vert_index+0, p1);
        mesh->set_vert_coord(vert_index+1, p2);
        mesh->set_vert_coord(vert_index+2, p3);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;

        // half upper-girdle 2
        glm::vec3 p4 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *upper_girdle_inner_rim_radius
                +glm::vec3(0, upper_girdle_inner_rim_y, 0);
        glm::vec3 p5 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thin_part_top_y, 0);
        glm::vec3 p6 = orient_to_offset(glm::vec3(
                0,
                0,                              // pitch
                static_cast<float>(i+1)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_top_y, 0);
        mesh->set_vert_coord(vert_index+0, p4);
        mesh->set_vert_coord(vert_index+1, p5);
        mesh->set_vert_coord(vert_index+2, p6);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;
    }

    // girdle: 16*2 triangles
    for(int i=0; i<16; i++) {
        float left_top_y     = (i%2)     ? girdle_thin_part_top_y    : girdle_thick_part_top_y;
        float left_bottom_y  = (i%2)     ? girdle_thin_part_bottom_y : girdle_thick_part_bottom_y;
        float right_top_y    = ((i+1)%2) ? girdle_thin_part_top_y    : girdle_thick_part_top_y;
        float right_bottom_y = ((i+1)%2) ? girdle_thin_part_bottom_y : girdle_thick_part_bottom_y;

        // top half
        glm::vec3 p1 = orient_to_offset(glm::vec3(
                0,
                0,                               // pitch
                static_cast<float>(i+1)/16*360)) // yaw
                *radius
                +glm::vec3(0, right_top_y, 0);
        glm::vec3 p2 = orient_to_offset(glm::vec3(
                0,
                0,                             // pitch
                static_cast<float>(i)/16*360)) // yaw
                *radius
                +glm::vec3(0, left_top_y, 0);
        glm::vec3 p3 = orient_to_offset(glm::vec3(
                0,
                0,                             // pitch
                static_cast<float>(i)/16*360)) // yaw
                *radius
                +glm::vec3(0, left_bottom_y, 0);
        mesh->set_vert_coord(vert_index+0, p1);
        mesh->set_vert_coord(vert_index+1, p2);
        mesh->set_vert_coord(vert_index+2, p3);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;

        // bottom half
        glm::vec3 p4 = orient_to_offset(glm::vec3(
                0,
                0,                             // pitch
                static_cast<float>(i)/16*360)) // yaw
                *radius
                +glm::vec3(0, left_bottom_y, 0);
        glm::vec3 p5 = orient_to_offset(glm::vec3(
                0,
                0,                               // pitch
                static_cast<float>(i+1)/16*360)) // yaw
                *radius
                +glm::vec3(0, right_bottom_y, 0);
        glm::vec3 p6 = orient_to_offset(glm::vec3(
                0,
                0,                               // pitch
                static_cast<float>(i+1)/16*360)) // yaw
                *radius
                +glm::vec3(0, right_top_y, 0);
        mesh->set_vert_coord(vert_index+0, p4);
        mesh->set_vert_coord(vert_index+1, p5);
        mesh->set_vert_coord(vert_index+2, p6);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;
    }

    // lower-girdle: 16 triangles
    for(int i=0; i<8; i++) {
        // half lower-girdle 1
        glm::vec3 p1 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *lower_girdle_inner_rim_radius
                +glm::vec3(0, lower_girdle_inner_rim_y, 0);
        glm::vec3 p2 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thin_part_bottom_y, 0);
        glm::vec3 p3 = orient_to_offset(glm::vec3(
                0,
                0,                            // pitch
                static_cast<float>(i)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_bottom_y, 0);
        mesh->set_vert_coord(vert_index+0, p1);
        mesh->set_vert_coord(vert_index+1, p2);
        mesh->set_vert_coord(vert_index+2, p3);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;

        // half lower-girdle 2
        glm::vec3 p4 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *lower_girdle_inner_rim_radius
                +glm::vec3(0, lower_girdle_inner_rim_y, 0);
        glm::vec3 p5 = orient_to_offset(glm::vec3(
                0,
                0,                              // pitch
                static_cast<float>(i+1)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_bottom_y, 0);
        glm::vec3 p6 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thin_part_bottom_y, 0);
        mesh->set_vert_coord(vert_index+0, p4);
        mesh->set_vert_coord(vert_index+1, p5);
        mesh->set_vert_coord(vert_index+2, p6);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;
    }

    // pavilion main: 8*2 triangles
    for(int i=0; i<8; i++) {
        // half pavilion main 1
        glm::vec3 p1 = glm::vec3(0, 0, 0);
        glm::vec3 p2 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *lower_girdle_inner_rim_radius
                +glm::vec3(0, lower_girdle_inner_rim_y, 0);
        glm::vec3 p3 = orient_to_offset(glm::vec3(
                0,
                0,                            // pitch
                static_cast<float>(i)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_bottom_y, 0);
        mesh->set_vert_coord(vert_index+0, p1);
        mesh->set_vert_coord(vert_index+1, p2);
        mesh->set_vert_coord(vert_index+2, p3);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;

        // half pavilion main 2
        glm::vec3 p4 = glm::vec3(0, 0, 0);
        glm::vec3 p5 = orient_to_offset(glm::vec3(
                0,
                0,                              // pitch
                static_cast<float>(i+1)/8*360)) // yaw
                *radius
                +glm::vec3(0, girdle_thick_part_bottom_y, 0);
        glm::vec3 p6 = orient_to_offset(glm::vec3(
                0,
                0,                                // pitch
                static_cast<float>(i+0.5)/8*360)) // yaw
                *lower_girdle_inner_rim_radius
                +glm::vec3(0, lower_girdle_inner_rim_y, 0);
        mesh->set_vert_coord(vert_index+0, p4);
        mesh->set_vert_coord(vert_index+1, p5);
        mesh->set_vert_coord(vert_index+2, p6);
        mesh->set_tri_indices(tri_index++, glm::uvec3(vert_index+0, vert_index+1, vert_index+2));
        vert_index += 3;
    }

    for(int i=0; i<static_cast<int>(mesh->get_num_tri()); i++) {
        glm::vec3 p0 = mesh->get_vert_coord(i*3+0);
        glm::vec3 p1 = mesh->get_vert_coord(i*3+1);
        glm::vec3 p2 = mesh->get_vert_coord(i*3+2);
        glm::vec3 e1 = glm::normalize(p1-p0);
        glm::vec3 e2 = glm::normalize(p2-p0);
        glm::vec3 n = glm::normalize(glm::cross(e1, e2));
        for(int j=0; j<3; j++) {
            mesh->set_vert_normal( i*3+j, n);
            mesh->set_vert_tangent(i*3+j, e1);
        }
    }

    return mesh;
}

}
