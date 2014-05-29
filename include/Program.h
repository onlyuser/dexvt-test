#ifndef VT_PROGRAM_H_
#define VT_PROGRAM_H_

#include <IdentObject.h>
#include <GL/glew.h>

namespace vt {

class Shader;
class VarAttribute;
class VarUniform;

class Program : public IdentObject
{
public:
    Program();
    virtual ~Program();
    void attach_shader(const Shader* shader) const;
    bool link() const;
    void use() const;
    VarAttribute* get_var_attribute(const GLchar *name) const;
    VarUniform* get_var_uniform(const GLchar *name) const;
    void get_program_iv(
            GLenum pname,
            GLint *params) const;
};

}

#endif
