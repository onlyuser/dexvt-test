#include <FrameBuffer.h>
#include <Texture.h>
#include <GL/glew.h>
#include <iostream>

namespace vt {

FrameBuffer::FrameBuffer(Texture* texture)
    : m_texture(texture),
      m_depthrenderbuffer_id(0)
{
    glGenFramebuffers(1, &m_id);
    bind();

    // The depth buffer
    glGenRenderbuffers(1, &m_depthrenderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthrenderbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_texture->width(), m_texture->height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer_id);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->id(), 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "failed to generate frame buffer" << std::endl;
    }
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer::bind() const
{
    glBindFramebuffer(m_target, m_id);
    glViewport(0, 0, m_texture->width(), m_texture->height());
}

}
