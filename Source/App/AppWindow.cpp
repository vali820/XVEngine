#include "AppWindow.hpp"

#include "App.hpp"
#include "Window/Window.hpp"

#ifdef __linux__
#include "Window/WlWindow.hpp"
#include "Window/X11Window.hpp"
#endif

Vec<u8> readFile(const char* path) {
    std::ifstream f(path, std::ios::ate | std::ios::binary);
    u64 size = f.tellg();
    Vec<u8> buf(size);
    f.seekg(0);
    f.read((char*)buf.getData(), (i64)size);
    f.close();
    return buf;
}

AppWindow::AppWindow(App* _app, const String& title) : app(_app), device(app->getDevice()) {
#ifdef __linux__
    if (app->getWindowingSystem() == WindowingSystem::Wayland) {
        window = new WlWindow(app->wlConnection);
    } else if (app->getWindowingSystem() == WindowingSystem::X11) {
        window = new X11Window(app->x11Connection, 800, 600);
    }
#endif

    window->setTitle(title);
    window->setExitCallback([this]() { app->exit(); });
    window->setResizeCallback([this](const ResizeEvent& e) {
        width = e.width, height = e.height;
        config.width = width, config.height = height;
        renderingInfo.renderArea.extent = {width, height};
        surface->configure(config);
    });

    surface       = new Surface(device, window);
    surfaceFormat = surface->getSupportedFormats()[0];
    config        = {
               .format      = surfaceFormat.format,
               .colorSpace  = surfaceFormat.colorSpace,
               .presentMode = VK_PRESENT_MODE_FIFO_KHR,
               .width       = 800,
               .height      = 600,
    };
    surface->configure(config);

    queue     = device->getGraphicsQueue();
    cmdBuffer = new CmdBuffer(device->getGraphicsCmdPool());

    imageAvailable = new Semaphore(device);
    renderFinished = new Semaphore(device);
    fence          = new Fence(device, true);

    renderingInfo = {
        .renderArea       = {{0, 0}, {width, height}},
        .colorAttachments = {RenderingAttachment{
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue  = {.color = {.float32 = {0.1f, 0.1f, 0.1f, 1.0f}}},
        }},
    };
    ShaderDesc vsDesc = {
        .stage     = VK_SHADER_STAGE_VERTEX_BIT,
        .nextStage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .codeType  = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .code      = readFile("../vs.srv"),
        .name      = "main",
    };
    ShaderDesc fsDesc = {
        .stage     = VK_SHADER_STAGE_FRAGMENT_BIT,
        .nextStage = 0,
        .codeType  = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .code      = readFile("../fs.srv"),
        .name      = "main",
    };

    vs = new Shader(device, vsDesc);
    fs = new Shader(device, fsDesc);

    Vec<Vertex> vertices{
        {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
    };

    u64 verticesSize = vertices.getSize() * sizeof(Vertex);
    vertexBuffer     = new Buffer(device,
                              verticesSize,
                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                              VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
    std::memcpy(vertexBuffer->getData(), vertices.getData(), verticesSize);
}

AppWindow::~AppWindow() {
    delete vertexBuffer;
    delete vs, delete fs;
    delete imageAvailable, delete renderFinished, delete fence;
    delete cmdBuffer;
    delete surface;
    delete window;
}

void AppWindow::update() {
    fence->waitFor(UINT64_MAX);
    fence->reset();

    imageIndex = surface->getNextImageIndex(UINT64_MAX, imageAvailable, nullptr);

    renderingInfo.colorAttachments[0].imageView = surface->getImageViews()[imageIndex];

    cmdBuffer->begin();

    cmdBuffer->setViewport({0.0f, 0.0f, (f32)width, (f32)height, 0.0f, 1.0f});
    cmdBuffer->setScissor({{0, 0}, {width, height}});
    cmdBuffer->setRasterizerDiscardEnable(false);

    cmdBuffer->setVertexInput({VertexBinding{
                                  .binding   = 0,
                                  .stride    = sizeof(Vertex),
                                  .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                              }},
                              {VertexAttribute{
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
                               }});
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
    cmdBuffer->setColorWriteMask(0, {COLOR_COMPONENTS_ALL});

    cmdBuffer->bindShader(VK_SHADER_STAGE_VERTEX_BIT, vs);
    cmdBuffer->bindShader(VK_SHADER_STAGE_FRAGMENT_BIT, fs);

    cmdBuffer->bindVertexBuffer(vertexBuffer, 0);

    cmdBuffer->beginRendering(renderingInfo);
    cmdBuffer->draw(3, 1, 0, 0);
    cmdBuffer->endRendering();

    cmdBuffer->end();

    queue->submit({cmdBuffer},
                  {imageAvailable},
                  {renderFinished},
                  {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                  fence);

    queue->present({renderFinished}, surface, imageIndex);
}
