#ifndef MGP_SPIRV_UTIL_COMMON_H_
#define MGP_SPIRV_UTIL_COMMON_H_

#include "rhi/Shader.h"
#include "rhi/Resource.h"

namespace arhi {

bool compileGLSL(const char* source, ShaderStage stage, std::vector<uint32_t>& spirvSource);
bool spirvReflect(std::vector<uint32_t>& spirvSource, ShaderStage stage, ShaderModuleReflection& reflection);

bool spirvToWGSL(const char* source, int sourceLen, std::string& wgslCode);
bool compileWGSL(const char* source, ShaderStage stage, std::vector<uint32_t>& spirvSource);
}
#endif