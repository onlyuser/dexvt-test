#ifndef VT_PRIMITIVE_FACTORY_H_
#define VT_PRIMITIVE_FACTORY_H_

namespace vt {

class Mesh;

class PrimitiveFactory
{
public:
    static Mesh* create_grid(int cols, int rows, float width = 1, float height = 1);
    static Mesh* create_sphere(int slices, int stacks, float radius = 1);
    static Mesh* create_hemisphere(int slices, int stacks, float radius = 1);
    static Mesh* create_cylinder(int slices, float radius = 1, float height = 1);
    static Mesh* create_cone(int slices, float radius = 1, float height = 1);
    static Mesh* create_torus(int slices, int stacks, float radius_major = 1, float radius_minor = 0.5);
    static Mesh* create_box(float width = 1, float height = 1, float length = 1);
    static Mesh* create_skybox()
    {
        return create_grid(1, 1);
    }
};

}

#endif
