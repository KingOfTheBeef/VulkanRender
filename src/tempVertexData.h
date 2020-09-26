//
// Created by jonat on 14/07/2020.
//

#ifndef DYNAMICLINK_TEMPVERTEXDATA_H
#define DYNAMICLINK_TEMPVERTEXDATA_H

#include <vulkan/vulkan.h>
#include "GraphicMath.h"

namespace Data {

    namespace Cube {

        static float cubeModel[] = {
                // Bottom                   // Colours
                -5.0f, 10.0f, 5.0f, 1.0f,   1.0f, 0.0f, 0.0f, 0.0f,  // 0
                5.0f, 10.0f, 5.0f, 1.0f,    0.0f, 1.0f, 0.0f, 0.0f,  // 1
                5.0f, 10.0f, -5.0f, 1.0f,   0.0f, 0.0f, 1.0f, 0.0f,  // 2
                -5.0f, 10.0f, -5.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0.0f,  // 3
                // Top
                -5.0f, 0.0f, 5.0f, 1.0f,    0.5f, 0.65f, 0.0f, 0.0f,  // 4
                5.0f, 0.0f, 5.0f, 1.0f,     0.21f, 0.1f, 0.34f, 0.0f,  // 5
                5.0f, 0.0f, -5.0f, 1.0f,    1.f, 1.0f, 0.0f, 0.0f,  // 6
                -5.0f, 0.0f, -5.0f, 1.0f,   0.0f, 1.0f, 1.0f, 0.0f   // 7
        };

        static int vertexStride = sizeof(float) * 8;

        static int positionOffset = sizeof(float) * 0;
        static int colourOffset   = sizeof(float) * 4;

        static uint16_t cubeIndex[] = {
                // Top Face
                4, 5, 6, 4, 6, 7,
                // Front face
                7, 3, 2, 7, 6, 2,
                // Left face
                7, 0, 4, 3, 0, 7,
                // Right face
                6, 5, 1, 6, 1, 2,
                // Back face
                4, 5, 1, 4, 1, 0,
                // Bottom face
                0, 3, 2, 0, 2, 1
        };
        static int cubeIndexCount = 36;

        static GMATH::mat4 instanceViews[] = {
                GMATH::identityMatrix(),
                GMATH::rotateMatrix(GMATH::vec3(1.0f, 1.0f, 0.5f), 180.0f, GMATH::translateMatrix(GMATH::vec3(10.0f, -20.0f, 0.0f))),
                GMATH::rotateMatrix(GMATH::vec3(1.0f, 1.0f, 0.5f), 180.0f, GMATH::translateMatrix(GMATH::vec3(-10.0f, 20.0f, 0.0f))),
                GMATH::rotateMatrix(GMATH::vec3(1.0f, 7.0f, 0.3f), 180.0f, GMATH::translateMatrix(GMATH::vec3(-30.0f, 0.0f, 0.0f))),
                GMATH::rotateMatrix(GMATH::vec3(5.0f, 1.0f, 0.5f), 45.0f, GMATH::translateMatrix(GMATH::vec3(15.0f, 0.0f, 0.0f))),
                GMATH::rotateMatrix(GMATH::vec3(0.0f, 0.2f, 0.7f), 78.0f, GMATH::translateMatrix(GMATH::vec3(-20.0f, -20.0f, 0.0f)))
        };

        static GMATH::vec4 quaternionRotations[] = {
                GMATH::vec4(0.26f, 0.00f, 0.79f, 0.55f),
                GMATH::vec4(0.92f, 0.00f, 0.40f, 0.00f),
                GMATH::vec4(0.21f, 0.57f, 0.65f, 0.45f)
        };

        static int instanceCount = 1;

        static float instanceStride = sizeof(GMATH::vec4);

        static int instanceOffset = {
                0
        };

        /*
        static int instanceCount = 6;

        static float instanceStride = sizeof(GMATH::mat4);

        static int instanceOffsets[4] = {
                sizeof(float[4]) * 0, sizeof(float[4]) * 1, sizeof(float[4]) * 2, sizeof(float[4]) * 3
        };
         */

    }

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
    static int instanceCount = 5;
    static float instanceData[] = {
        15.0f, -5.0f,
        0.0f, 0.0f,
        20.0f, 10.f,
        -15.6f, 32.5f,
        24.0f, -13.0f
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
