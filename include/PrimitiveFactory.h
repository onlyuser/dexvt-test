#ifndef VT_PRIMITIVE_FACTORY_H_
#define VT_PRIMITIVE_FACTORY_H_

#include <string>

#define DEFAULT_SLICES 16
#define DEFAULT_STACKS 16

namespace vt {

class Mesh;

class PrimitiveFactory
{
public:
    static Mesh* create_grid(
            std::string name   = "",
            int         cols   = 1,
            int         rows   = 1,
            float       width  = 1,
            float       height = 1);
    static Mesh* create_sphere(
            std::string name = "",
            int         slices = DEFAULT_SLICES,
            int         stacks = DEFAULT_STACKS,
            float       radius = 1);
    static Mesh* create_hemisphere(
            std::string name   = "",
            int         slices = DEFAULT_SLICES,
            int         stacks = DEFAULT_STACKS,
            float       radius = 1);
    static Mesh* create_cylinder(
            std::string name   = "",
            int         slices = DEFAULT_SLICES,
            float       radius = 1,
            float       height = 1);
    static Mesh* create_cone(
            std::string name   = "",
            int         slices = DEFAULT_SLICES,
            float       radius = 1,
            float       height = 1);
    static Mesh* create_torus(
            std::string name         = "",
            int         slices       = DEFAULT_SLICES,
            int         stacks       = DEFAULT_STACKS,
            float       radius_major = 1,
            float       radius_minor = 0.5);
    static Mesh* create_box(
            std::string name   = "",
            float       width  = 1,
            float       height = 1,
            float       length = 1);
    static Mesh* create_tetrahedron(
            std::string name   = "",
            float       width  = 1,
            float       height = 1,
            float       length = 1);
    static Mesh* create_viewport_quad(std::string name)
    {
        return create_grid(name, 1, 1);
    }
};

}

#endif
