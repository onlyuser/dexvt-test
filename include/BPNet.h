#ifndef VT_BPNET_H_
#define VT_BPNET_H_

#include <Texture.h>
#include <glm/glm.hpp>
#include <vector>

namespace vt {

class BPNetLayer
{
public:
    BPNetLayer(size_t width);
    ~BPNetLayer();
    Texture* get_weight_texture() const { return m_weight_texture; }
    size_t get_width() const            { return m_width; }

private:
    Texture* m_weight_texture;
    size_t   m_width;
};

class BPNet
{
public:
    BPNet(size_t layer_count, size_t width);
    ~BPNet();
    BPNetLayer* get_layer(int index)      { return m_layers[index]; }
    Texture*    get_value_texture() const { return m_value_texture; }
    Texture*    get_error_texture() const { return m_error_texture; }

private:
    std::vector<BPNetLayer*> m_layers;
    Texture*                 m_value_texture;
    Texture*                 m_error_texture;
};

}

#endif
