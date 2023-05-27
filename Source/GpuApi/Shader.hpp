#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"

class Device;

class Shader {
   private:
    Device* device;
    VkShaderEXT shader{};
    VkPipelineLayout pipelineLayout{};

    VkShaderStageFlags stage;

   public:
    Shader(Device* device, const ShaderDesc& desc);

    ~Shader();

    Vec<u8> getBinaryData();

    inline Device* getDevice() { return device; }
    inline VkShaderEXT getVkShader() { return shader; }
    inline VkPipelineLayout getVkPipelineLayout() { return pipelineLayout; }
    [[nodiscard]] inline VkShaderStageFlags getStage() const { return stage; }
};
