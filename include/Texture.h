#ifndef VT_TEXTURE_H_
#define VT_TEXTURE_H_

#include <NamedObject.h>
#include <ViewObject.h>
#include <IdentObject.h>
#include <BindableObjectIFace.h>
#include <GL/glew.h>
#include <string>
#include <glm/glm.hpp>

#define DEFAULT_TEXTURE_WIDTH  256
#define DEFAULT_TEXTURE_HEIGHT 256

namespace vt {

class Texture : public NamedObject,
                public ViewObject<glm::ivec2, size_t>,
                public IdentObject,
                public BindableObjectIFace
{
public:
    typedef enum { RGB, DEPTH, STENCIL } type_t;

    Texture(std::string          name       = "",
            glm::ivec2           dim        = glm::vec2(DEFAULT_TEXTURE_WIDTH,
                                                        DEFAULT_TEXTURE_HEIGHT),
            const unsigned char* pixel_data = NULL,
            type_t               type       = Texture::RGB,
            bool                 smooth     = true,
            bool                 random     = false);
    Texture(
            std::string name,
            std::string png_filename,
            bool        smooth = true);
    Texture(
            std::string name,
            std::string png_filename_pos_x,
            std::string png_filename_neg_x,
            std::string png_filename_pos_y,
            std::string png_filename_neg_y,
            std::string png_filename_pos_z,
            std::string png_filename_neg_z);
    virtual ~Texture();
    void bind();
    type_t get_type() const
    {
        return m_type;
    }

private:
    bool   m_skybox;
    type_t m_type;

    static GLuint gen_texture_internal(
            size_t      width,
            size_t      height,
            const void* pixel_data,
            type_t      type   = Texture::RGB,
            bool        smooth = true);
    static GLuint gen_texture_skybox_internal(
            size_t      width,
            size_t      height,
            const void* pixel_data_pos_x,
            const void* pixel_data_neg_x,
            const void* pixel_data_pos_y,
            const void* pixel_data_neg_y,
            const void* pixel_data_pos_z,
            const void* pixel_data_neg_z);
    static bool read_png(std::string png_filename, void** pixel_data, size_t* width, size_t* height);
};

}

#endif
