
#pragma once
#include "../config.h"

class Renderer
{
    public:
        Renderer(MTL::Device* device);
        ~Renderer();
        void draw(MTK::View* view);
    void buildPipeline();
    private:
        MTL::Device* device;
        MTL::CommandQueue* commandQueue;
        MTL::RenderPipelineState* trianglePipeline;
};
