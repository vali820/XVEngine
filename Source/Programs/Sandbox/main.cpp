#include <fstream>

#include "App/Window/Window.hpp"
#include "Core/Core.hpp"
#include "Core/Math.hpp"
#include "GpuApi/GpuApi.hpp"

struct Vertex {
    FVec3 pos;
    FVec3 color;
};

class App {
   private:
    Ptr<Window> window;

    Device* device;
    Surface* surface;
    SurfaceConfig config{};
    CmdBuffer* cmdBuffer;

    CmdPool* pool;
    Queue* queue;

    Semaphore* imageAvailable;
    Semaphore* renderFinished;
    Fence* fence;

    Shader* vs;
    Shader* fs;

    VkSurfaceFormatKHR surfaceFormat{};
    RenderingInfo renderingInfo;
    u32 imageIndex = 0;

    u32 width = 800, height = 600;

    bool exit = false;

   public:
    App() {
        window        = Window::create(width, height);
        device        = new Device;
        surface       = new Surface(device, window.get());
        surfaceFormat = surface->getSupportedFormats()[0];
        config        = {
                   .format      = surfaceFormat.format,
                   .colorSpace  = surfaceFormat.colorSpace,
                   .presentMode = VK_PRESENT_MODE_FIFO_KHR,
                   .width       = 800,
                   .height      = 600,
        };
        surface->configure(config);

        pool          = device->getGraphicsCmdPool();
        queue         = device->getGraphicsQueue();

        renderingInfo = {
            .renderArea       = {{0, 0}, {width, height}},
            .colorAttachments = {RenderingAttachment{
                .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue  = {.color = {.float32 = {0.1f, 0.1f, 0.1f, 1.0f}}},
            }},
        };

        imageAvailable = new Semaphore(device);
        renderFinished = new Semaphore(device);
        fence          = new Fence(device, true);

        cmdBuffer      = new CmdBuffer(pool);

        vs             = new Shader(device,
                                    {
                                        .stage     = VK_SHADER_STAGE_VERTEX_BIT,
                                        .nextStage = VK_SHADER_STAGE_FRAGMENT_BIT,
                                        .codeType  = VK_SHADER_CODE_TYPE_SPIRV_EXT,
                                        .code      = readFile("../vs.srv"),
                                        .name      = "main",
                        });
        fs             = new Shader(device,
                                    {
                                        .stage     = VK_SHADER_STAGE_FRAGMENT_BIT,
                                        .nextStage = 0,
                                        .codeType  = VK_SHADER_CODE_TYPE_SPIRV_EXT,
                                        .code      = readFile("../fs.srv"),
                                        .name      = "main",
                        });
    }

    ~App() {
        delete vs;
        delete fs;
        delete cmdBuffer;
        delete surface;
        delete imageAvailable;
        delete renderFinished;
        delete fence;
        delete device;
    };

    void run() {
        window->setExitCallback([this]() { exit = true; });

        window->setResizeCallback([this](const ResizeEvent& e) {
            width = e.width, height = e.height;
            config.width = width, config.height = height;
            renderingInfo.renderArea.extent = {width, height};
            surface->configure(config);
        });

        window->setKeyPressedCallback([](const KeyPressedEvent& e) { println("{} {}", e.key, e.modifiers); });

        window->setTitle("Engine");

        while (!exit) {
            window->update();
            update();
        }

        device->waitIdle();
    }

   private:
    static Vec<u8> readFile(const char* path) {
        std::ifstream f(path, std::ios::ate | std::ios::binary);
        u64 size = f.tellg();
        Vec<u8> buf(size);
        f.seekg(0);
        f.read((char*)buf.getData(), (i64)size);
        f.close();
        return buf;
    }

    void update() {
        fence->waitFor(UINT64_MAX);
        fence->reset();

        imageIndex = surface->getNextImageIndex(UINT64_MAX, imageAvailable, nullptr);

        renderingInfo.colorAttachments[0].imageView = surface->getImageViews()[imageIndex];

        recordCmdBuffer();

        queue->submit({cmdBuffer},
                      {imageAvailable},
                      {renderFinished},
                      {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                      fence);

        queue->present({renderFinished}, surface, imageIndex);
    }

    void recordCmdBuffer() {
        cmdBuffer->begin();

        cmdBuffer->setViewport({0.0f, 0.0f, (f32)width, (f32)height, 0.0f, 1.0f});
        cmdBuffer->setScissor({{0, 0}, {width, height}});
        cmdBuffer->setRasterizerDiscardEnable(false);

        cmdBuffer->setVertexInput(
            {
                VertexBinding{
                    .binding   = 0,
                    .stride    = sizeof(Vertex),
                    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                },
            },
            {
                VertexAttribute{
                    .location = 0,
                    .binding  = 0,
                    .offset   = 0,
                    .format   = VkFormat::VK_FORMAT_R32G32B32_SFLOAT,
                },
                VertexAttribute{
                    .location = 1,
                    .binding  = 0,
                    .offset   = sizeof(FVec3),
                    .format   = VkFormat::VK_FORMAT_R32G32B32_SFLOAT,
                },
            });
        cmdBuffer->setPrimitiveTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        cmdBuffer->setPrimitiveRestartEnable(false);

        cmdBuffer->setRasterizationSamples(VK_SAMPLE_COUNT_1_BIT);
        cmdBuffer->setSampleMask(VK_SAMPLE_COUNT_1_BIT, {1});
        cmdBuffer->setAlphaToCoverageEnable(false);
        cmdBuffer->setPolygonMode(VK_POLYGON_MODE_FILL);
        cmdBuffer->setCullMode(VK_CULL_MODE_BACK_BIT);
        cmdBuffer->setFrontFace(VK_FRONT_FACE_CLOCKWISE);
        cmdBuffer->setDepthTestEnable(false);
        cmdBuffer->setDepthWriteEnable(false);
        cmdBuffer->setDepthBiasEnable(false);
        cmdBuffer->setStencilTestEnable(false);

        cmdBuffer->setColorBlendEnable(0, {false});
        cmdBuffer->setColorWriteMask(0,
                                     {VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                      VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT});

        cmdBuffer->bindShader(VK_SHADER_STAGE_VERTEX_BIT, vs);
        cmdBuffer->bindShader(VK_SHADER_STAGE_FRAGMENT_BIT, fs);

        cmdBuffer->beginRendering(renderingInfo);
        cmdBuffer->draw(3, 1, 0, 0);
        cmdBuffer->endRendering();

        cmdBuffer->end();
    }
};

int main() {
    App app;
    app.run();
}
