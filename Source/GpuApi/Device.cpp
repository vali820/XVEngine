#include "Device.hpp"

#include <vulkan/vulkan.h>

#include "CmdBuffer.hpp"
#include "Queue.hpp"

static const Vec<const char*> enabledLayers = {
#ifndef NDEBUG
    "VK_LAYER_KHRONOS_validation",
#endif
};
static const Vec<const char*> instanceExtensions = {
    "VK_KHR_surface",
#ifdef __linux__
    "VK_KHR_xcb_surface",
    "VK_KHR_wayland_surface",
#endif
};
static const Vec<const char*> deviceExtensions = {
    "VK_KHR_swapchain",
    "VK_EXT_shader_object",
    "VK_EXT_extended_dynamic_state3",
};

Device::Device() {
    createInstance();
    pickPhysicalDevice();
    setupQueueCreateInfos();
    createDevice();
    createAllocator();
    getFunctionPointers();
    createCommandPools();
}

Device::~Device() {
    delete graphicsCmdPool;
    delete computeCmdPool;
    delete transferCmdPool;

    delete graphicsQueue;
    delete computeQueue;
    delete transferQueue;

    vmaDestroyAllocator(allocator);

    vkDestroyDevice(device, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Device::createInstance() {
    VkApplicationInfo appInfo{
        .sType      = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo createInfo{
        .sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo        = &appInfo,
        .enabledLayerCount       = (u32)enabledLayers.getSize(),
        .ppEnabledLayerNames     = enabledLayers.getData(),
        .enabledExtensionCount   = (u32)instanceExtensions.getSize(),
        .ppEnabledExtensionNames = instanceExtensions.getData(),
    };

    vkCreateInstance(&createInfo, nullptr, &instance);
}

void Device::pickPhysicalDevice() {
    u32 deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    Vec<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.getData());

    for (auto pd : devices) {
        getPhysicalDeviceFeatures(pd);

        // clang-format off
        bool featuresOk = supportedFeatures2.features.fillModeNonSolid and
                          supportedVulkan13Features.synchronization2 and
                          supportedVulkan13Features.dynamicRendering and
                          supportedShaderObjectFeatures.shaderObject and
                          supportedDescriptorBufferFeatures.descriptorBuffer;
        // clang-format on

        if (featuresOk)
            physicalDevice = pd;
        else
            throw std::runtime_error("Failed to find suitable device");
    }
}

void Device::getPhysicalDeviceFeatures(VkPhysicalDevice pd) {
    supportedDescriptorBufferFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT,
        .pNext = nullptr,
    };
    supportedShaderObjectFeatures = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_OBJECT_FEATURES_EXT,
        .pNext = &supportedDescriptorBufferFeatures,
    };
    supportedVulkan13Features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &supportedShaderObjectFeatures,
    };
    supportedFeatures2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &supportedVulkan13Features,
    };

    vkGetPhysicalDeviceFeatures2(pd, &supportedFeatures2);
}

void Device::setupQueueCreateInfos() {
    u32 queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    Vec<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.getData());

    i32 qfIdx           = 0;
    u32 prioritiesCount = 0;
    for (const auto& qf : queueFamilies) {
        bool isValid = false;
        // We assume that any graphics queue also supports presenting
        if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT and graphicsQueueFamily == -1) {
            graphicsQueueFamily = qfIdx;
            isValid             = true;
        }

        if (qf.queueFlags & VK_QUEUE_COMPUTE_BIT and computeQueueFamily == -1) {
            computeQueueFamily = qfIdx;
            isValid            = true;
        }

        if (qf.queueFlags & VK_QUEUE_TRANSFER_BIT and transferQueueFamily == -1) {
            transferQueueFamily = qfIdx;
            isValid             = true;
        }

        if (isValid) {
            VkDeviceQueueCreateInfo createInfo{
                .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = (u32)qfIdx,
                .queueCount       = qf.queueCount,
            };
            prioritiesCount += qf.queueCount;
            queueCreateInfos.push(createInfo);
        }

        qfIdx++;
    }

    queuePriorities = Vec<f32>(prioritiesCount, 1.0f);
    u32 idx         = 0;
    for (auto& qInfo : queueCreateInfos) {
        qInfo.pQueuePriorities  = queuePriorities.getData() + idx;
        idx                    += qInfo.queueCount;
    }
}

void Device::createDevice() {
    VkDeviceCreateInfo createInfo = {
        .sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext                   = &enabledFeatures2,
        .queueCreateInfoCount    = (u32)queueCreateInfos.getSize(),
        .pQueueCreateInfos       = queueCreateInfos.getData(),
        .enabledExtensionCount   = (u32)deviceExtensions.getSize(),
        .ppEnabledExtensionNames = deviceExtensions.getData(),
        .pEnabledFeatures        = nullptr,
    };

    vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);

    graphicsQueue = new Queue(this, graphicsQueueFamily);
    computeQueue  = new Queue(this, computeQueueFamily);
    transferQueue = new Queue(this, transferQueueFamily);
}

void Device::createAllocator() {
    VmaAllocatorCreateInfo createInfo{
        .physicalDevice   = physicalDevice,
        .device           = device,
        .instance         = instance,
        .vulkanApiVersion = VK_API_VERSION_1_3,
    };
    vmaCreateAllocator(&createInfo, &allocator);
}

#define GETCMD(x) x = (PFN_##x)vkGetDeviceProcAddr(device, #x)
void Device::getFunctionPointers() {
    GETCMD(vkCmdSetVertexInputEXT);
    GETCMD(vkCmdSetRasterizationSamplesEXT);
    GETCMD(vkCmdSetSampleMaskEXT);
    GETCMD(vkCmdSetAlphaToCoverageEnableEXT);
    GETCMD(vkCmdSetPolygonModeEXT);
    GETCMD(vkCmdSetColorBlendEnableEXT);
    GETCMD(vkCmdSetColorBlendEquationEXT);
    GETCMD(vkCmdSetColorWriteMaskEXT);

    GETCMD(vkCmdBindShadersEXT);
    GETCMD(vkCreateShadersEXT);
    GETCMD(vkDestroyShaderEXT);
    GETCMD(vkGetShaderBinaryDataEXT);
}

void Device::createCommandPools() {
    VkCommandPoolCreateInfo createInfo{
        .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = (u32)graphicsQueueFamily,
    };
    VkCommandPool gPool;
    VkCommandPool cPool;
    VkCommandPool tPool;

    // Graphics command pool
    vkCreateCommandPool(device, &createInfo, nullptr, &gPool);
    graphicsCmdPool = new CmdPool(this, gPool);

    // Compute command pool
    if (computeQueueFamily == graphicsQueueFamily) {
        computeCmdPool = new CmdPool(this, gPool, false);
    } else {
        createInfo.queueFamilyIndex = (u32)computeQueueFamily;
        vkCreateCommandPool(device, &createInfo, nullptr, &cPool);
        computeCmdPool = new CmdPool(this, cPool);
    }

    // Transfer command pool
    if (transferQueueFamily == graphicsQueueFamily)
        transferCmdPool = new CmdPool(this, gPool, false);
    else if (transferQueueFamily == computeQueueFamily)
        transferCmdPool = new CmdPool(this, cPool, false);
    else {
        createInfo.queueFamilyIndex = (u32)transferQueueFamily;
        vkCreateCommandPool(device, &createInfo, nullptr, &tPool);
        transferCmdPool = new CmdPool(this, tPool);
    }
}

void Device::waitIdle() { vkDeviceWaitIdle(device); }
