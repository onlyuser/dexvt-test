#include <FrameBuffer.h>
#include <Texture.h>
#include <Camera.h>
#include <GL/glew.h>
#include <iostream>

namespace vt {

FrameBuffer::FrameBuffer(Texture* texture, Camera* camera)
    : m_texture(texture),
      m_camera(camera),
      m_depthrenderbuffer_id(0)
{
    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    glGenRenderbuffers(1, &m_depthrenderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthrenderbuffer_id);
    if(texture->type() == Texture::STENCIL) {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, m_texture->width(), m_texture->height());
    } else {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_texture->width(), m_texture->height());
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if(texture->type() == Texture::DEPTH) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture->id(), 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    } else if(texture->type() == Texture::STENCIL) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->id(), 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer_id);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer_id);
    } else {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->id(), 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthrenderbuffer_id);
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

void FrameBuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_camera->get_width(), m_camera->get_height());
}

}
