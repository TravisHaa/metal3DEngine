
#pragma once
#include "../config.h"

class Renderer
{
    public:
        Renderer(MTL::Device* device);
        ~Renderer();
        void draw(MTK::View* view);
    private:
        void buildMeshes();
        void buildShaders();
        MTL::RenderPipelineState* buildShader(const char* filename, const char* vertName, const char* fragName);
        MTL::Device* device;
        MTL::CommandQueue* commandQueue;
        MTL::Buffer* triangleMesh;
        MTL::RenderPipelineState* trianglePipeline, *generalPipeline;
};
