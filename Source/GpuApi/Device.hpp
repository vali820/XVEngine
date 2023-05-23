#pragma once

#include <vulkan/vulkan.h>

#include "Common.hpp"
#include "Core/Core.hpp"

#define DEFCMD(x) \
    PFN_##x x {}

class Queue;
class CmdPool;

class Device {
   private:
    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};

    VkPhysicalDeviceShaderObjectFeaturesEXT supportedShaderObjectFeatures{};
    VkPhysicalDeviceVulkan13Features supportedVulkan13Features{};
    VkPhysicalDeviceFeatures2 supportedFeatures2{};

    VkPhysicalDeviceShaderObjectFeaturesEXT enabledShaderObjectFeatures{
        .sType        = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
        .pNext        = nullptr,
        .shaderObject = true,
    };
    VkPhysicalDeviceVulkan13Features enabledVulkan13Features{
        .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext            = &enabledShaderObjectFeatures,
        .synchronization2 = true,
        .dynamicRendering = true,
    };
    VkPhysicalDeviceFeatures2 enabledFeatures2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &enabledVulkan13Features,
        .features =
            {
                .fillModeNonSolid = true,
            },
    };

    i32 graphicsQueueFamily = -1;
    i32 computeQueueFamily  = -1;
    i32 transferQueueFamily = -1;

    Vec<f32> queuePriorities;
    Vec<VkDeviceQueueCreateInfo> queueCreateInfos;

    Queue* graphicsQueue{};
    Queue* computeQueue{};
    Queue* transferQueue{};

    CmdPool* graphicsCmdPool{};
    CmdPool* computeCmdPool{};
    CmdPool* transferCmdPool{};

   public:
    DEFCMD(vkCmdSetVertexInputEXT);
    DEFCMD(vkCmdSetRasterizationSamplesEXT);
    DEFCMD(vkCmdSetSampleMaskEXT);
    DEFCMD(vkCmdSetAlphaToCoverageEnableEXT);
    DEFCMD(vkCmdSetPolygonModeEXT);
    DEFCMD(vkCmdSetColorBlendEnableEXT);
    DEFCMD(vkCmdSetColorBlendEquationEXT);
    DEFCMD(vkCmdSetColorWriteMaskEXT);

    DEFCMD(vkCmdBindShadersEXT);
    DEFCMD(vkCreateShadersEXT);
    DEFCMD(vkDestroyShaderEXT);
    DEFCMD(vkGetShaderBinaryDataEXT);

    Device();
    ~Device();

    void waitIdle();

    inline Queue* getGraphicsQueue() { return graphicsQueue; }
    inline Queue* getComputeQueue() { return computeQueue; }
    inline Queue* getTransferQueue() { return transferQueue; }

    inline CmdPool* getGraphicsCmdPool() { return graphicsCmdPool; }
    inline CmdPool* getComputeCmdPool() { return computeCmdPool; }
    inline CmdPool* getTransferCmdPool() { return transferCmdPool; }

    inline VkDevice getVkDevice() { return device; }
    inline VkInstance getVkInstance() { return instance; }
    inline VkPhysicalDevice getVkPhysicalDevice() { return physicalDevice; }

   private:
    void createInstance();
    void pickPhysicalDevice();
    void getPhysicalDeviceFeatures(VkPhysicalDevice pd);
    void setupQueueCreateInfos();
    void createDevice();
    void getFunctionPointers();
    void createCommandPools();
};

#undef DEFCMD