#ifndef WGBuffer_H_
#define WGBuffer_H_

#include "WGDevice.h"

namespace arhi {

class WGBuffer : public Buffer {
    WGDevice* device = nullptr;
public:
    uint64_t size;
    WGPUBuffer buffer;

    ~WGBuffer();
    void init(WGDevice* device, const BufferDesc* desc);
    void setData(int offset, const void* data, int size) override;
};

}
#endif