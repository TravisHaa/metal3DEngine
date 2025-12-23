

#include "renderer.h"
#include "mesh_factory.h"

Renderer::Renderer(MTL::Device* device):
device(device->retain())
{
    //job of GPU to create command queue
    commandQueue = device->newCommandQueue();
    buildShaders();
    buildMeshes();
}
//any metal obj is manually managed
Renderer::~Renderer() {
    triangleMesh->release();
    trianglePipeline->release();
    generalPipeline->release();
    commandQueue->release();
    device->release();
}

void Renderer::buildMeshes() {
    triangleMesh = MeshFactory::build_triangle(device);
}

void Renderer::buildShaders() {
    trianglePipeline = buildShader("shaders/triangle.metal", "vertexMain", "fragmentMain");
    generalPipeline = buildShader("shaders/general.metal", "vertexMainGeneral", "fragmentMainGeneral");
}

MTL::RenderPipelineState* Renderer::buildShader(
    const char* filename, const char* vertName, const char* fragName) {
    std::cout << "--- Reading File ---" << std::endl;
    
    std::ifstream file;
    file.open(filename);
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
    NS::String* vertexName = NS::String::string(vertName, NS::StringEncoding::UTF8StringEncoding);
    MTL::Function* vertexMain = lib->newFunction(vertexName);
    
    //vertex determine actual point, while fragment determiens all other attibutes: color, etc
    NS::String* fragmentName = NS::String::string(fragName , NS::StringEncoding::UTF8StringEncoding);
    MTL::Function* fragmentMain = lib->newFunction(fragmentName);
    
    //need to make render pipeline descriptor - recipe for how GPU should render
    MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    descriptor->setVertexFunction(vertexMain);
    descriptor->setFragmentFunction(fragmentMain);
    descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    
    //building our actual triangle render pipeline
    MTL::RenderPipelineState* pipeline = device->newRenderPipelineState(descriptor, &error);
    if(!pipeline) {
        std::cout << error->localizedDescription()->utf8String() << std::endl;
    }
    
    descriptor->release();
    vertexMain->release();
    fragmentMain->release();
    lib->release();
    file.close();
    
    return pipeline;
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
    encoder->setVertexBuffer(triangleMesh, 0, 0 );
    
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
