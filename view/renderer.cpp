

#include "renderer.h"

Renderer::Renderer(MTL::Device* device):
device(device->retain())
{
    //job of GPU to create command queue
    commandQueue = device->newCommandQueue();
    buildPipeline();
}
//any metal obj is manually managed
Renderer::~Renderer() {
    commandQueue->release();
    device->release();
}

void Renderer::buildPipeline() {
    std::cout << "--- Reading File ---" << std::endl;
    
    std::ifstream file;
    file.open("shaders/triangle.metal");
    if (!file.is_open()) {
        std::cout << "FAILED TO OPEN FILE\n";
        return;
    }
    std::stringstream reader;
    reader << file.rdbuf();
    std::string raw_str = reader.str();
    std::cout << raw_str;
    
    NS::String* source_code = NS::String::string(raw_str.c_str(), NS::StringEncoding::UTF8StringEncoding);
    
    //metal library obj
    NS::Error* error = nullptr;
    MTL::CompileOptions* options = nullptr;
    MTL::Library* lib = device->newLibrary(source_code, options, &error);
    if(!lib) {
        std::cout << error->localizedDescription()->utf8String() << std::endl;
    }
    
//need to manually free these, specify name of function and fetch it from lib
    NS::String* vertexName = NS::String::string("vertexMain", NS::StringEncoding::UTF8StringEncoding);
    MTL::Function* vertexMain = lib->newFunction(vertexName);
    
    NS::String* fragmentName = NS::String::string("fragmentMain", NS::StringEncoding::UTF8StringEncoding);
    MTL::Function* fragmentMain = lib->newFunction(fragmentName);
    
    //need to make render pipeline descriptor
    MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    descriptor->setVertexFunction(vertexMain);
    descriptor->setFragmentFunction(fragmentMain);
    descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    
    //building our actual triangle render pipeline
    trianglePipeline = device->newRenderPipelineState(descriptor, &error);
    if(!trianglePipeline) {
        std::cout << error->localizedDescription()->utf8String() << std::endl;
    }
    
    descriptor->release();
    vertexMain->release();
    fragmentMain->release();
    lib->release();
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
    
    //set pipeline state, and use shader
    encoder->setRenderPipelineState(trianglePipeline);
    
    //defines vertex IDs 0-3 , MTL primitive triangle takes 3 points and fills its interior in
    encoder->drawPrimitives(
                            MTL::PrimitiveTypeTriangle,
                             NS::UInteger(0), NS::UInteger(3));
     
     
    //clear screen and present graphics
    encoder->endEncoding();
    commandBuffer->presentDrawable(view->currentDrawable());
    commandBuffer->commit();
    
    pool->release();
}
