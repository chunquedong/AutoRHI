#include "WGBuffer.h"

using namespace arhi;

void WGBuffer::init(WGDevice* device, const BufferDesc* desc) {
    this->device = device;
    WGPUBufferDescriptor iboDesc = { .usage = (WGPUBufferUsage)desc->usage, .size = (uint64_t)desc->size, };
    buffer = wgpuDeviceCreateBuffer(device->device, &iboDesc);
    this->size = desc->size;
}
void WGBuffer::setData(int offset, const void* data, int size) {
    wgpuQueueWriteBuffer(device->queue, this->buffer, offset, data, size);
}

WGBuffer::~WGBuffer() {
    wgpuBufferRelease(buffer);
}