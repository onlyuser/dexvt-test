#ifndef VT_FRAME_BUFFER_H_
#define VT_FRAME_BUFFER_H_

#include <IdentObject.h>
#include <BindableObjectIFace.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace vt {

class Texture;
class Camera;

class FrameBuffer : public IdentObject, public BindableObjectIFace
{
public:
    FrameBuffer(Texture* texture, Camera* camera);
    virtual ~FrameBuffer();
    void bind();
    void unbind();

private:
    Texture* m_texture;
    Camera* m_camera;
    GLuint m_depthrenderbuffer_id;
    glm::vec2 m_prev_viewport_dim;
};

}

#endif
