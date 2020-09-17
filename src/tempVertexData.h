//
// Created by jonat on 14/07/2020.
//

#ifndef DYNAMICLINK_TEMPVERTEXDATA_H
#define DYNAMICLINK_TEMPVERTEXDATA_H

#include <vulkan/vulkan.h>

namespace Data {

    static float indexedVertexData[] = {
            // Coords                       // UVs
            -10.0f, -10.0f, 0.0f, 1.0f,     0.0f, 0.0f,     // top left
            10.0f, -10.0f, 0.0f, 1.0f,      1.0f, 0.0f,     // top right
            10.0f, 10.0f, 0.0f, 1.0f,       1.0f, 1.0f,     // bottom right
            -10.0f, 10.0f, 0.0f, 1.0f,      0.0f, 1.0f      // bottom left
    };

    static uint16_t indexData[] = {
        0, 1, 2, 0, 3, 2
    };

    // 2D coords for each instance
    static float instanceData[] = {
        15.0f, -5.0f,
        0.0f, 0.0f
    };


    static float vertexData[] = {
            // Position                  // Color
            0.0f, 0.3f, 0.0f, 1.0f,     0.5f, 0.65f, 0.0f, 0.0f,
            -0.7f, 0.5f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.7f, 0.0f, 1.0f,     0.5f, 1.0f, 0.0f, 0.0f,
            0.7f, 0.5f, 0.0f, 1.0f,     1.0f, 1.0f, 0.0f, 0.0f,

            0.7f, -0.5f, 0.0f, 1.0f,    1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.7f, 0.0f, 1.0f,    0.5f, 0.0f, 0.0f, 0.0f,
            -0.7f, -0.5f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 0.0f,
            -0.7f, 0.5f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f
    };

    static float altVertexData[] = {
            // Position                  // Color
            0.0f, -0.3f, 0.0f, 1.0f,    1.0f, 0.6f, 0.0f, 0.0f,
            -0.7f, -0.5f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, -0.7f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,
            0.7f, -0.5f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,

            0.7f, 0.5f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.7f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f, 0.0f,
            -0.7f, 0.5f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,
            -0.7f, -0.5f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f, 0.0f
    };

    static size_t vertexDataSize = sizeof(vertexData);
    static VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    static uint32_t vertexCount = 8;
    static uint32_t stride = sizeof(float[8]);
    static uint32_t positionOffset = 0;
    static uint32_t colorOffset = sizeof(float[4]);
}
#endif //DYNAMICLINK_TEMPVERTEXDATA_H
