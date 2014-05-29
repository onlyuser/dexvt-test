#include <Buffer.h>
#include <GL/glew.h>

namespace vt {

Buffer::Buffer(GLenum target, size_t size, void* data)
    : m_target(target),
      m_size(size)
{
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(target, size, data, GL_STATIC_DRAW);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &m_id);
}

void Buffer::bind() const
{
    glBindBuffer(m_target, m_id);
}

}
