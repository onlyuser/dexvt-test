#ifndef VT_BINDABLE_OBJECT_IFACE_H_
#define VT_BINDABLE_OBJECT_IFACE_H_

namespace vt {

class BindableObjectIFace
{
public:
    virtual void bind() const = 0;
};

}

#endif
