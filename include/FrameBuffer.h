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

private:
    Texture* m_texture;
    GLuint m_depthrenderbuffer_id;
};

}

#endif
