add_library(GpuApi
        Common.cpp Device.cpp Queue.cpp CmdBuffer.cpp
        Surface.cpp
        Fence.cpp Semaphore.cpp
        Shader.cpp Descriptor.cpp
        Buffer.cpp Image.cpp
        ../ThirdParty/vma/vk_mem_alloc.cpp
        )

target_link_libraries(GpuApi Core App vulkan)
