
//#pragma once
//#include <Metal/Metal.hpp>
//#include <AppKit/AppKit.hpp>
//#include <MetalKit/MetalKit.hpp>
#pragma once

#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>
#include <AppKit/AppKit.hpp>
#include <Foundation/Foundation.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include <simd/simd.h>

struct Vertex {
    simd::float2 pos;
    simd::float3 color;
};
