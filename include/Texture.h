#ifndef VT_TEXTURE_H_
#define VT_TEXTURE_H_

#include <IdentObject.h>
#include <BindableObjectIFace.h>
#include <GL/glew.h>
#include <string>

namespace vt {

class Texture : public IdentObject, public BindableObjectIFace
{
public:
    Texture(std::string name, const void* pixel_data, size_t width, size_t height);
    Texture(std::string name, std::string png_filename);
    Texture(
            std::string name,
            std::string png_filename_pos_x,
            std::string png_filename_neg_x,
            std::string png_filename_pos_y,
            std::string png_filename_neg_y,
            std::string png_filename_pos_z,
            std::string png_filename_neg_z);
    virtual ~Texture();
    void bind() const;
    const std::string &get_name() const
    {
        return m_name;
    }

private:
    std::string m_name;
    bool m_skybox;

    static GLuint gen_texture_internal(const void* pixel_data, size_t width, size_t height);
    static GLuint gen_texture_skybox_internal(
            const void* pixel_data_pos_x,
            const void* pixel_data_neg_x,
            const void* pixel_data_pos_y,
            const void* pixel_data_neg_y,
            const void* pixel_data_pos_z,
            const void* pixel_data_neg_z,
            size_t width,
            size_t height);
    static bool read_png(std::string png_filename, void **pixel_data, size_t *width, size_t *height);
};

}

#endif
