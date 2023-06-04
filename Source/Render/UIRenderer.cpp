#include "UIRenderer.hpp"

Vec<u8> readFile(const char *path) {
    std::ifstream f(path, std::ios::ate | std::ios::binary);
    if (!f) throw std::runtime_error(std::format("Failed to open file: {}", path));
    u64 size = f.tellg();
    Vec<u8> buf(size);
    f.seekg(0);
    f.read((char *)buf.getData(), (i64)size);
    f.close();
    return buf;
}

UIRenderer::UIRenderer(Device *_device, Surface *_surface, u32 _width, u32 _height)
    : device(_device), surface(_surface), width(_width), height(_height) {
    queue     = device->getGraphicsQueue();
    cmdBuffer = new CmdBuffer(device->getGraphicsCmdPool());

    renderingInfo = {
        .renderArea       = {{0, 0}, {width, height}},
        .colorAttachments = {RenderingAttachment{
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp      = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp     = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue  = {.color = {.float32 = {0.0, 0.0, 0.0, 1.0}}},
        }},
    };

    fence = new Fence(device, true);

    ShaderDesc vsDesc = {
        .stage     = VK_SHADER_STAGE_VERTEX_BIT,
        .nextStage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .codeType  = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .code      = readFile("Shaders/UIVertex.vert.spv"),
        .name      = "main",
    };
    ShaderDesc roundedBoxDesc = {
        .stage              = VK_SHADER_STAGE_FRAGMENT_BIT,
        .nextStage          = 0,
        .codeType           = VK_SHADER_CODE_TYPE_SPIRV_EXT,
        .code               = readFile("Shaders/UIRoundedBox.frag.spv"),
        .name               = "main",
        .pushConstantRanges = {VkPushConstantRange{
            .offset = 0,
            .size   = sizeof(UIDrawCmdRoundedBox),
        }},
    };

    vs               = new Shader(device, vsDesc);
    roundedBoxShader = new Shader(device, roundedBoxDesc);

    Vec<Vec2> vertices = {
        {-1.0, -1.0},
        {1.0, -1.0},
        {-1.0, 1.0},
        {-1.0, 1.0},
        {1.0, -1.0},
        {1.0, 1.0},
    };
    vertexBuffer = new CpuVisibleBuffer(device, 6 * sizeof(Vec2), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    memcpy(vertexBuffer->getData(), vertices.getData(), 6 * sizeof(Vec2));
}

UIRenderer::~UIRenderer() {
    delete vertexBuffer;
    delete vs, delete roundedBoxShader;
    delete fence;
    delete cmdBuffer;
}

void UIRenderer::render(const UIDrawData &drawData, u32 imageIndex, Semaphore *imageAvailable,
                        Semaphore *renderFinished) {
    fence->waitFor(UINT64_MAX);
    fence->reset();

    renderingInfo.colorAttachments[0].imageView = surface->getImageViews()[imageIndex];

    recordCommandBuffer(drawData);

    queue->submit({cmdBuffer},
                  {imageAvailable},
                  {renderFinished},
                  {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT},
                  fence);
}

void UIRenderer::resize(u32 _width, u32 _height) {
    width = _width, height = _height;
    renderingInfo.renderArea.extent = {width, height};
}

void UIRenderer::recordCommandBuffer(const UIDrawData &drawData) {
    cmdBuffer->begin();
    cmdBuffer->defaultState();

    cmdBuffer->setViewport({0.0, 0.0, (f32)width, (f32)height, 0.0, 1.0});
    cmdBuffer->setScissor({{0, 0}, {width, height}});

    cmdBuffer->setVertexInput({VertexBinding{
                                  .binding   = 0,
                                  .stride    = sizeof(Vec2),
                                  .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                              }},
                              {VertexAttribute{
                                  .location = 0,
                                  .binding  = 0,
                                  .offset   = 0,
                                  .format   = VK_FORMAT_R32G32_SFLOAT,
                              }});

    cmdBuffer->setColorBlendEnable(0, true);
    cmdBuffer->setColorBlendEquation(0,
                                     VkColorBlendEquationEXT{
                                         .srcColorBlendFactor = VK_BLEND_FACTOR_DST_ALPHA,
                                         .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                                         .colorBlendOp        = VK_BLEND_OP_ADD,
                                         .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
                                         .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                                         .alphaBlendOp        = VK_BLEND_OP_ADD,
                                     });

    cmdBuffer->bindShader(VK_SHADER_STAGE_VERTEX_BIT, vs);
    cmdBuffer->bindVertexBuffer(vertexBuffer, 0);

    cmdBuffer->beginRendering(renderingInfo);

    for (auto &cmd : drawData.getDrawCommands()) {
        switch (cmd.kind) {
            case UIDrawCmdKind::RoundedBox: {
                // setVertexBufferRect(cmd.roundedBox.rect);

                cmdBuffer->bindShader(VK_SHADER_STAGE_FRAGMENT_BIT, roundedBoxShader);
                cmdBuffer->pushConstant(roundedBoxShader, 0, sizeof(UIDrawCmdRoundedBox), &cmd.roundedBox);
                cmdBuffer->draw(6, 1, 0, 0);
                break;
            }
            default: break;
        }
    }

    cmdBuffer->endRendering();

    cmdBuffer->end();
}

void UIRenderer::setVertexBufferRect(const Rect &rect) {
    Vec2 size          = {(f32)width, (f32)height};
    auto [x, y]        = rect.getPosition() / size * 2.0 - 1.0;
    auto [w, h]        = rect.getSize() / size * 2.0;
    Vec<Vec2> vertices = {{x, y}, {x + w, y}, {x, y + h}, {x + w, y}, {x + w, y + h}, {x, y + h}};
    memcpy(vertexBuffer->getData(), vertices.getData(), 6 * sizeof(Vec2));
}