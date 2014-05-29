#ifndef VT_SHADER_H_
#define VT_SHADER_H_

#include <IdentObject.h>
#include <GL/glew.h>
#include <string>

namespace vt {

class Shader : public IdentObject
{
public:
    Shader(std::string filename, GLenum type);
    virtual ~Shader();
};

}

#endif
