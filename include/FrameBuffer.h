#ifndef VT_FRAME_BUFFER_H_
#define VT_FRAME_BUFFER_H_

#include <IdentObject.h>
#include <BindableObjectIFace.h>
#include <GL/glew.h>

namespace vt {

class Texture;
class Camera;

class FrameBuffer : public IdentObject, public BindableObjectIFace
{
public:
    FrameBuffer(Texture* texture, Camera* camera);
    virtual ~FrameBuffer();
    void bind() const;
    void unbind() const;

private:
    Texture* m_texture;
    Camera* m_camera;
    GLuint m_depthrenderbuffer_id;
};

}

#endif
