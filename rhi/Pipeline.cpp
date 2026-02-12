#include "Pipeline.h"

using namespace arhi;

void Pipeline::reflect(const PipelineDesc* desc) {
    Shader* vertexShader = (desc->vertexShader);

    for (int i = 0; i < vertexShader->reflection.uniforms.size(); ++i) {
        const UniformVar& uniform = vertexShader->reflection.uniforms[i];
        reflection.uniforms[uniform.name] = uniform;
    }

    if (desc->fragmentShader) {
        Shader* fragmentShader = (desc->fragmentShader);
        for (int i = 0; i < fragmentShader->reflection.uniforms.size(); ++i) {
            const UniformVar& uniform = fragmentShader->reflection.uniforms[i];
            auto found = reflection.uniforms.find(uniform.name);
            if (found != reflection.uniforms.end()) {
                found->second.visibility |= uniform.visibility;
            }
            else {
                reflection.uniforms[uniform.name] = uniform;
            }
        }
    }

    for (int j = 0; j < vertexShader->reflection.inputAttrs.size(); ++j) {
        const AttributeVar& shaderInfo = vertexShader->reflection.inputAttrs[j];
        std::string name = shaderInfo.name;
        reflection.attributesIndex[name] = shaderInfo;
        reflection.attributes[shaderInfo.location] = shaderInfo;
    }
}