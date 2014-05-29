#include <Shader.h>
#include <../../common/include/shader_utils.h>
#include <GL/glew.h>
#include <string>

namespace vt {

Shader::Shader(std::string filename, GLenum type)
{
    m_id = create_shader(filename.c_str(), type);
}

Shader::~Shader()
{
    glDeleteShader(m_id);
}

}
