//
//  triangle.metal
//  hello_metal_cpp
//
//  Created by Travis Ha on 12/19/25.
//

#include <metal_stdlib>
using namespace metal;

//hard coded positions
constant float4 positions[] = {
    //xyz ranges from -1 to 1, with 0 being center
    ///x,y,z positive z goes into screen
    float4(-0.75, -0.75, 0, 1.0),
    float4(0.75, -0.75, 0, 1.0),
    float4(0, 1, 0, 1.0),
    
};

constant half3 colors[] = {
    half3(1,0, 0),
    half3(0,0,1.0),
    half3(0,0,1.0),
};

//vertexID [[[]] signals metal api to fill in with vertex ID variable

struct VertexPayload {
    float4 position [[position]];
    half3 color; //half precision floats increases speed of math operations
    
};
//qualify vertex functions with keyword vertex
VertexPayload vertex vertexMain(uint vertexID [[vertex_id]]) {
    VertexPayload payload;
    payload.position = positions[vertexID];
    payload.color = colors[vertexID];
    return payload; //sent to rasterizer
}
//stage in qualifies that this is an input var, takes in interpolated payload from rasterizer
half4 fragment fragmentMain(VertexPayload frag [[stage_in]]) {
    return half4(frag.color, 1.0); //return payload's color
}
