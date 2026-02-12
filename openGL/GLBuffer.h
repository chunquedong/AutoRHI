#ifndef GLBuffer_H_
#define GLBuffer_H_

#include "GLDevice.h"

namespace arhi {

class GLBuffer : public Buffer {
    GLDevice* device = nullptr;
    BufferDesc desc;
public:
    GLuint buffer = 0;

    ~GLBuffer();
    void init(GLDevice* device, const BufferDesc* desc);
    void setData(int offset, const void* data, int size) override;
};

}
#endif