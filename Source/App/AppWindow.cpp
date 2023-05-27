#include "AppWindow.hpp"

#include "App.hpp"
#include "GpuApi/GpuApi.hpp"
#include "Window/Window.hpp"
#include "Window/WindowConnection.hpp"

Vec<u8> readFile(const char* path) {
    std::ifstream f(path, std::ios::ate | std::ios::binary);
    u64 size = f.tellg();
    Vec<u8> buf(size);
    f.seekg(0);
    f.read((char*)buf.getData(), (i64)size);
    f.close();
    return buf;
}

void changeImageLayout(CmdBuffer* cmd, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
                       VkPipelineStageFlags2 srcStageMask, VkPipelineStageFlags2 dstStageMask,
                       VkAccessFlags2 srcAccessMask, VkAccessFlags2 dstAccessMask) {
    VkImageMemoryBarrier2 barrier{
        .sType         = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .srcStageMask  = srcStageMask,
        .srcAccessMask = srcAccessMask,
        .dstStageMask  = dstStageMask,
        .dstAccessMask = dstAccessMask,
        .oldLayout     = oldLayout,
        .newLayout     = newLayout,
        .image         = image,
        .subresourceRange =
            {
                .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel   = 0,
                .levelCount     = 1,
                .baseArrayLayer = 0,
                .layerCount     = 1,
            },
    };
    cmd->barrier({barrier});
}

void prepareImageForPresent(CmdBuffer* cmd, VkImage image) {
    changeImageLayout(cmd,
                      image,
                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                      VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                      VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
                      VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
                      0);
}

void prepareImageForRender(CmdBuffer* cmd, VkImage image) {
    changeImageLayout(cmd,
                      image,
                      VK_IMAGE_LAYOUT_UNDEFINED,
                      VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                      VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
                      VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
                      0,
                      VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT);
}

AppWindow::AppWindow(App* _app, const String& title) : app(_app), device(app->getDevice()) {
    window = app->windowConnection->createWindow();

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
               .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
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
            .clearValue  = {.color = {.float32 = {0.0f, 0.0f, 0.0f, 0.0f}}},
        }},
    };
    ShaderDesc vsDesc = {
        .stage              = VK_SHADER_STAGE_VERTEX_BIT,
        .nextStage          = VK_SHADER_STAGE_FRAGMENT_BIT,
        .codeType           = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .code               = readFile("../vs.srv"),
        .name               = "main",
        .pushConstantRanges = {VkPushConstantRange{
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset     = 0,
            .size       = 4,
        }},
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

    Vec<u32> indices{0, 1, 2};

    vertexBuffer = createBufferLocal(
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertices.getData(), vertices.getSize() * sizeof(Vertex));
    indexBuffer = createBufferLocal(
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices.getData(), indices.getSize() * sizeof(u32));
}

AppWindow::~AppWindow() {
    delete descriptorSetLayout;
    delete vertexBuffer, delete indexBuffer;
    delete vs, delete fs;
    delete imageAvailable, delete renderFinished, delete fence;
    delete cmdBuffer;
    delete surface;
    delete window;
}

void AppWindow::update() {
    if (pushConstant > 1.0f) pushConstant = 0.0f;
    pushConstant += 0.01f;

    fence->waitFor(UINT64_MAX);
    fence->reset();

    surface->getNextImageIndex(UINT64_MAX, imageAvailable, nullptr, imageIndex);

    renderingInfo.colorAttachments[0].imageView = surface->getImageViews()[imageIndex];

    recordCommandBuffer();

    queue->submit({cmdBuffer},
                  {imageAvailable},
                  {renderFinished},
                  {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                  fence);

    queue->present({renderFinished}, surface, imageIndex);
}

void AppWindow::recordCommandBuffer() {
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
    cmdBuffer->bindIndexBuffer(indexBuffer, VK_INDEX_TYPE_UINT32);

    cmdBuffer->pushConstant(vs, 0, 4, &pushConstant);

    prepareImageForRender(cmdBuffer, surface->getImages()[imageIndex]);

    cmdBuffer->beginRendering(renderingInfo);
    cmdBuffer->drawIndexed(3, 1, 0, 0, 0);
    cmdBuffer->endRendering();

    prepareImageForPresent(cmdBuffer, surface->getImages()[imageIndex]);

    cmdBuffer->end();
}

Buffer* AppWindow::createBufferLocal(VkBufferUsageFlags usage, void* data, u64 size) {
    auto staging =
        new Buffer(device,
                   size,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT);
    memcpy(staging->getData(), data, size);
    auto buffer = new Buffer(device, size, usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0);

    auto cmd = new CmdBuffer(device->getGraphicsCmdPool());
    cmd->begin();
    cmd->copyBuffer(staging, buffer, size);
    cmd->end();
    queue->submit({cmd});
    queue->waitIdle();
    delete staging;
    return buffer;
}
