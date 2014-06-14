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
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    if(texture->depth_only()) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture->id(), 0);
    } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->id(), 0);
    }

    glGenRenderbuffers(1, &m_depthrenderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthrenderbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_texture->width(), m_texture->height());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer_id);

    if(texture->depth_only()) {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "failed to generate frame buffer" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_id);
}

void FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    glViewport(0, 0, m_texture->width(), m_texture->height());
}

void FrameBuffer::unbind(size_t width, size_t height) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}

}
