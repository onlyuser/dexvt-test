#ifndef VT_FRAME_BUFFER_H_
#define VT_FRAME_BUFFER_H_

#include <IdentObject.h>
#include <BindableObjectIFace.h>
#include <GL/glew.h>

namespace vt {

class Texture;

class FrameBuffer : public IdentObject, public BindableObjectIFace
{
public:
    FrameBuffer(Texture* texture);
    virtual ~FrameBuffer();
    void bind() const;
    size_t size() const
    {
        return m_size;
    }

private:
    GLenum m_target;
    size_t m_size;
    GLuint m_depthrenderbuffer_id;
    Texture* m_texture;
};

}

#endif
