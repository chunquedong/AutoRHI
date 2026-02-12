#include "rhi_spirv.h"

#include <SPIRV/GlslangToSpv.h>
#include <glslang/Include/glslang_c_interface.h>
#include <glslang/Public/ShaderLang.h>
//#include <glslang_c_api.h>
#include "spirv_reflect.h"


namespace arhi {

static EShLanguage wgpuShaderStageToGlslang(ShaderStage stage){
    if(stage == ShaderStage::Vertex) return EShLangVertex;
    //if(stage == ShaderStage::TessControl) return EShLangTessControl;
    //if(stage == ShaderStage::TessEvaluation) return EShLangTessEvaluation;
    if(stage == ShaderStage::Geometry) return EShLangGeometry;
    if(stage == ShaderStage::Fragment) return EShLangFragment;
    if(stage == ShaderStage::Compute) return EShLangCompute;
    //if(stage == ShaderStage::RayGen) return EShLangRayGen;
    //if(stage == ShaderStage::Intersect) return EShLangIntersect;
    //if(stage == ShaderStage::AnyHit) return EShLangAnyHit;
    //if(stage == ShaderStage::ClosestHit) return EShLangClosestHit;
    //if(stage == ShaderStage::Miss) return EShLangMiss;
    //if(stage == ShaderStage::Callable) return EShLangCallable;
    //if(stage == ShaderStage::Task) return EShLangTask;
    //if(stage == ShaderStage::Mesh) return EShLangMesh;
    MGP_ERROR("Unsupport shader stage: %d", stage);
    return (EShLanguage)~0;
}

static std::vector<uint32_t> glsl_to_spirv_single(const char* nullterminatedSource, EShLanguage stage, glslang::EShTargetClientVersion targetVulkanVersion, glslang::EShTargetLanguageVersion targetSpirvVersion){
    
    glslang::TShader shader(stage);
    shader.setEnvInput (glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, targetVulkanVersion);
    shader.setEnvClient(glslang::EShClientVulkan, targetVulkanVersion);
    shader.setEnvTarget(glslang::EShTargetSpv, targetSpirvVersion);
    //const char* nullterminatedSource = nullptr;
    const char* sources[2] = {
        "#version 450\n", nullterminatedSource
    };
    shader.setStrings(sources, 2);

    auto stageToString = [](EShLanguage stage){
        switch(stage){
            case EShLangVertex: return "EShLangVertex";
            case EShLangTessControl: return "EShLangTessControl";
            case EShLangTessEvaluation: return "EShLangTessEvaluation";
            case EShLangGeometry: return "EShLangGeometry";
            case EShLangFragment: return "EShLangFragment";
            case EShLangCompute: return "EShLangCompute";
            case EShLangRayGen: return "EShLangRayGen";
            case EShLangIntersect: return "EShLangIntersect";
            case EShLangAnyHit: return "EShLangAnyHit";
            case EShLangClosestHit: return "EShLangClosestHit";
            case EShLangMiss: return "EShLangMiss";
            case EShLangCallable: return "EShLangCallable";
            case EShLangTask: return "EShLangTask";
            case EShLangMesh: return "EShLangMesh";
            default: return "??unknown ShaderStage??";
        }
    };

    TBuiltInResource Resources = {};
    Resources.maxComputeWorkGroupSizeX = 1024;
    Resources.maxComputeWorkGroupSizeY = 1024;
    Resources.maxComputeWorkGroupSizeZ = 1024;
    Resources.maxCombinedTextureImageUnits = 8;

    Resources.limits.generalUniformIndexing = true;
    Resources.limits.generalVariableIndexing = true;
    Resources.maxDrawBuffers = 4;

    EShMessages messages = (EShMessages)(EShMsgDefault | EShMsgSpvRules | EShMsgVulkanRules);
    //char errorBuffer[2048];
    
    if(!shader.parse(&Resources, targetVulkanVersion, ECoreProfile, false, false, messages)){
        MGP_ERROR("%s GLSL Parsing Failed: %s", stageToString(stage), shader.getInfoLog());
        //puts(errorBuffer);
    }
    else{
        glslang::TProgram program;
        program.addShader(&shader);
        if(!program.link(messages)){
            MGP_ERROR("Error linkin shader: %s", program.getInfoLog());
            //puts(errorBuffer);
        }
        glslang::TIntermediate* intermediate = program.getIntermediate(stage);
        std::vector<uint32_t> output;

        glslang::SpvOptions options;
        options.generateDebugInfo = true;
        options.stripDebugInfo = false;
        glslang::GlslangToSpv(*intermediate, output, &options);
        /*if(nts){
            free(nts);
        }*/
        return output;
    }
    return std::vector<uint32_t>{};
}

static int spvImageTypeToSampleType(const SpvImageFormat type_desc, bool is_depth_image) {
    if (is_depth_image) {
        return 4;
    }
    //if (type_desc->type_flags & SPV_REFLECT_TYPE_FLAG_FLOAT) {
    //    // SPIRV-Reflect doesn't directly distinguish filterable vs unfilterable for float.
    //    // This often depends on the format and usage. Assume filterable for simplicity here.
    //    // You might need more sophisticated logic based on SpvImageFormat.
    //    return WGPUTextureSampleType_Float;
    //}
    //if (type_desc->type_flags & SPV_REFLECT_TYPE_FLAG_INT) {
    //    if (type_desc->traits.numeric.scalar.signedness) {
    //        return WGPUTextureSampleType_Sint;
    //    } else {
    //        return WGPUTextureSampleType_Uint;
    //    }
    //}
    return 2;
}

static TextureType spvDimToViewDimension(SpvDim dim, bool arrayed) {
    switch (dim) {
    case SpvDim1D: return TextureType::_1D; // WebGPU doesn't have 1D array textures
    case SpvDim2D: return arrayed ? TextureType::_2DArray : TextureType::_2D;
    case SpvDim3D: return TextureType::_3D; // 3D textures cannot be arrayed
    case SpvDimCube: return arrayed ? TextureType::CubeArray : TextureType::Cube;
    default: return TextureType::Undefined;
    }
}

static bool getGlobalRI(SpvReflectShaderModule mod, std::vector<UniformVar>& uniforms) {

    uint32_t descriptorSetCount = 0;
    SpvReflectResult result = spvReflectEnumerateDescriptorSets(&mod, &descriptorSetCount, NULL);
    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        MGP_ERROR("Failed to enumerate descriptor sets (count)");
    }
    if (descriptorSetCount == 0) {
        return false;
    }

    SpvReflectDescriptorSet** descriptorSets = (SpvReflectDescriptorSet**)calloc(descriptorSetCount, sizeof(SpvReflectDescriptorSet*));
    if (descriptorSets == NULL) MGP_ERROR("Failed to allocate memory for descriptor set pointers");
    result = spvReflectEnumerateDescriptorSets(&mod, &descriptorSetCount, descriptorSets);
    if (result != SPV_REFLECT_RESULT_SUCCESS) MGP_ERROR("Failed to enumerate descriptor sets (pointers)");

    uint32_t totalGlobalCount = 0;
    for (uint32_t i = 0; i < descriptorSetCount; i++) {
        totalGlobalCount += descriptorSets[i]->binding_count;
    }

    if (totalGlobalCount == 0) {
        free((void*)descriptorSets); // Free the pointers array if no bindings.
        return false;
    }

    uint32_t globalInsertIndex = 0;
    for (uint32_t bindGroupIndex = 0; bindGroupIndex < descriptorSetCount; bindGroupIndex++) {
        const SpvReflectDescriptorSet* set = descriptorSets[bindGroupIndex];
        for (uint32_t entryIndex = 0; entryIndex < set->binding_count; entryIndex++) {
            const SpvReflectDescriptorBinding* entry = set->bindings[entryIndex];
            UniformVar insert = {}; // Initialize to zero

            insert.bindGroup = set->set; // Use the actual set number from reflection
            insert.binding = entry->binding;

            const char* entry_name_ptr = (entry->name && entry->name[0]) ? entry->name : (entry->type_description->type_name ? entry->type_description->type_name : "");
            insert.name = entry_name_ptr;
            // insert.stageFlags = entry->shader_stage_flags; // If WGPUGlobalReflectionInfo has stage flags

            switch (entry->descriptor_type) {
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                insert.type = UniformType::UniformBuffer;
                insert.bufferBindingType = BufferDesc::Usage_Uniform;
                insert.bufferMinBindingSize = entry->block.size; // Size of the UBO block
                break;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                insert.type = UniformType::StorageBuffer;
                insert.bufferBindingType = BufferDesc::Usage_Storage; // Or ReadOnlyStorage if distinguishable and needed
                insert.bufferMinBindingSize = entry->block.size; // Size of the SSBO block
                break;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                insert.type = UniformType::Sampler;
                insert.samplerBindingType = 2; // Default, see notes
                break;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                insert.type = UniformType::Texture;
                insert.textureSampleType = spvImageTypeToSampleType(entry->image.image_format, (entry->image.depth == 1));
                insert.textureViewDimension = (int)spvDimToViewDimension(entry->image.dim, entry->image.arrayed);
                break;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                // insert.resourceType = WGPUBindingResourceType_StorageTexture;
                /*insert.storageTexture.access = getStorageTextureAccess(entry);
                insert.storageTexture.format = spvImageFormatToWGPUFormat(entry->image.image_format);
                insert.storageTexture.viewDimension = spvDimToViewDimension(entry->image.dim, entry->image.arrayed);*/
                break;
            case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                // WebGPU requires separate texture and sampler.
                // This reflection info might need to create two entries or have a special type.
                // For now, matches your original wgvk_assert.
                MGP_ERROR("SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER not handled/allowed.");
                break;
                // Handle other types like SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT if necessary
            default:
                MGP_ERROR("Unhandled or illegal descriptor type in SPIR-V reflection.");
            }
            uniforms.push_back(insert);
        }
    }

    free((void*)descriptorSets); // Free the array of pointers (not the content they point to)
}

static AttributeVar spvReflectToReflectAttrib(SpvReflectInterfaceVariable* spvAttrib) {

    AttributeVar result = {};
    result.name = spvAttrib->name;
    result.format = spvAttrib->format;
    result.location = spvAttrib->location;
    switch (spvAttrib->format) {
    case SPV_REFLECT_FORMAT_R32_SFLOAT:
        //result.componentCount = 1;
        //result.bytePerComponent = 4;
        result.size = 4;
        break;
    case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
        //result.componentCount = 2;
        //result.bytePerComponent = 4;
        result.size = 8;
        break;
    case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
        //result.componentCount = 3;
        //result.bytePerComponent = 4;
        result.size = 12;
        break;
    case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
        //result.componentCount = 4;
        //result.bytePerComponent = 4;
        result.size = 16;
        break;
    case SPV_REFLECT_FORMAT_R32_UINT:
        //result.componentCount = 1;
        //result.bytePerComponent = 4;
        //result.isFloat = false;
        //result.isSigned = false;
        result.size = 4;
        break;
    case SPV_REFLECT_FORMAT_R32G32_UINT:
        //result.componentCount = 2;
        //result.bytePerComponent = 4;
        //result.isFloat = false;
        //result.isSigned = false;
        result.size = 8;
        break;
    case SPV_REFLECT_FORMAT_R32G32B32_UINT:
        //result.componentCount = 3;
        //result.bytePerComponent = 4;
        //result.isFloat = false;
        //result.isSigned = false;
        result.size = 12;
        break;
    case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
        /*result.componentCount = 4;
        result.bytePerComponent = 4;
        result.isFloat = false;
        result.isSigned = false;*/
        result.size = 16;
        break;
    case SPV_REFLECT_FORMAT_R32_SINT:
        /*result.componentCount = 1;
        result.bytePerComponent = 4;
        result.isFloat = false;
        result.isSigned = true;*/
        result.size = 4;
        break;
    case SPV_REFLECT_FORMAT_R32G32_SINT:
        /*result.componentCount = 2;
        result.bytePerComponent = 4;
        result.isFloat = false;
        result.isSigned = true;*/
        result.size = 8;
        break;
    case SPV_REFLECT_FORMAT_R32G32B32_SINT:
        /*result.componentCount = 3;
        result.bytePerComponent = 4;
        result.isFloat = false;
        result.isSigned = true;*/
        result.size = 12;
        break;
    case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
        /*result.componentCount = 4;
        result.bytePerComponent = 4;
        result.isFloat = false;
        result.isSigned = true;*/
        result.size = 16;
        break;
    default: break; //wgvk_assert(false, "Unhandled Spirv-reflect attribute type");
    }
    return result;
}

static int glslang_initialize_process_called = 0;

bool compileGLSL(const char* source, ShaderStage stage, std::vector<uint32_t>& spirvSource) {
    if (glslang_initialize_process_called == 0) {
        glslang::InitializeProcess();
        glslang_initialize_process_called = 1;
    }
    spirvSource = glsl_to_spirv_single(source, wgpuShaderStageToGlslang(stage), glslang::EShTargetVulkan_1_4, glslang::EShTargetSpv_1_4);
    return spirvSource.size() > 0;
}

bool spirvReflect(std::vector<uint32_t>& spirvSource, ShaderModuleReflection& reflectionRes) {
SpvReflectShaderModule mod{};
    SpvReflectResult result = spvReflectCreateShaderModule(spirvSource.size()*4, spirvSource.data(), &mod);

    if (result != SPV_REFLECT_RESULT_SUCCESS) {
        return false;
    }
    //SpvReflectDescriptorSet* descriptorSets = NULL;

    getGlobalRI(mod, reflectionRes.uniforms);

    SpvReflectInterfaceVariable** input_vars;
    SpvReflectInterfaceVariable** output_vars;

    uint32_t input_var_count = 0;
    uint32_t output_var_count = 0;
    spvReflectEnumerateInputVariables(&mod, &input_var_count, NULL);
    spvReflectEnumerateOutputVariables(&mod, &output_var_count, NULL);
    input_vars = (SpvReflectInterfaceVariable**)calloc(input_var_count, sizeof(uintptr_t));
    output_vars = (SpvReflectInterfaceVariable**)calloc(output_var_count, sizeof(uintptr_t));

    spvReflectEnumerateInputVariables(&mod, &input_var_count, input_vars);
    spvReflectEnumerateOutputVariables(&mod, &output_var_count, output_vars);

    for (uint32_t i = 0; i < input_var_count; i++) {
        SpvReflectInterfaceVariable* input_var_i = input_vars[i];
        reflectionRes.inputAttrs.push_back(spvReflectToReflectAttrib(input_var_i));
    }
    for (uint32_t i = 0; i < output_var_count; i++) {
        SpvReflectInterfaceVariable* output_var_i = output_vars[i];
        reflectionRes.outputAttrs.push_back(spvReflectToReflectAttrib(output_var_i));
    }

    //RL_FREE(descriptorSets);
    free((void*)input_vars);
    free((void*)output_vars);

    spvReflectDestroyShaderModule(&mod);
    return true;
}

}