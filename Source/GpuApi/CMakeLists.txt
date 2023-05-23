add_library(GpuApi SHARED
        Common.cpp Device.cpp Queue.cpp CmdBuffer.cpp
        Surface.cpp
        Fence.cpp Semaphore.cpp
        Shader.cpp DescriptorSet.cpp
        )

target_link_libraries(GpuApi Core vulkan)
