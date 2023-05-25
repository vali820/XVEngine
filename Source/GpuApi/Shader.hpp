#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"

class Device;

class Shader {
   private:
    Device* device;
    VkShaderEXT shader{};

   public:
    Shader(Device* device, const ShaderDesc& desc);

    ~Shader();

    Vec<u8> getBinaryData();

    inline Device* getDevice() { return device; }
    inline VkShaderEXT getVkShader() { return shader; }
};
