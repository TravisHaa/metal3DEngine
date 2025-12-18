

#include "renderer.h"

Renderer::Renderer(MTL::Device* device):
device(device->retain())
{
    //job of GPU to create command queue
    commandQueue = device->newCommandQueue();
}
//any metal obj is manually managed
Renderer::~Renderer() {
    commandQueue->release();
    device->release();
}

void Renderer::draw(MTK::View* view) {
    //creating objects here, adding to auto release pool so that it will be removed 
    NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
    
    //command buffer is list where draw commands are encoded and submitted to queue, presents drawings
    MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();
    //render pass displays whats on window, default behavior clears screen
    MTL::RenderPassDescriptor* renderPass = view->currentRenderPassDescriptor();
    //encoder records drawing commands, command buffer produces encoder, needs to know which renderPass we are drawing to
    MTL::RenderCommandEncoder* encoder = commandBuffer->renderCommandEncoder(renderPass);
    //clear screen and present graphics
    encoder->endEncoding();
    commandBuffer->presentDrawable(view->currentDrawable());
    commandBuffer->commit();
    
    pool->release();
}
