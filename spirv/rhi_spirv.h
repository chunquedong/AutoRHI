#ifndef MGP_SPIRV_UTIL_COMMON_H_
#define MGP_SPIRV_UTIL_COMMON_H_

#include "rhi/Shader.h"
#include "rhi/Resource.h"

namespace arhi {

bool compileGLSL(const char* source, ShaderStage stage, std::vector<uint32_t>& spirvSource);
bool spirvReflect(std::vector<uint32_t>& spirvSource, ShaderModuleReflection& reflection);

}
#endif