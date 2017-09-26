#include <BPNet.h>
#include <Texture.h>
#include <glm/glm.hpp>

namespace vt {

BPNetLayer::BPNetLayer(size_t width)
{
    m_weight_texture = new Texture("value_texture",
                                   vt::Texture::RGBA,
                                   glm::ivec2(width, width),
                                   false); // no smooth (we want values to be exact)
}

BPNetLayer::~BPNetLayer()
{
    delete m_weight_texture;
}

BPNet::BPNet(size_t layer_count, size_t width)
{
    for(int i = 0; i < static_cast<int>(layer_count); i++) {
        m_layers.push_back(new BPNetLayer(width));
    }
    m_value_texture = new Texture("value_texture",
                                  vt::Texture::RGBA,
                                  glm::ivec2(width, width),
                                  false); // no smooth (we want values to be exact)
    m_error_texture = new Texture("error_texture",
                                  vt::Texture::RGBA,
                                  glm::ivec2(width, width),
                                  false); // no smooth (we want values to be exact)
}

BPNet::~BPNet()
{
    for(std::vector<BPNetLayer*>::iterator p = m_layers.begin(); p != m_layers.end(); p++) {
        delete *p;
    }
    delete m_value_texture;
    delete m_error_texture;
}

}
