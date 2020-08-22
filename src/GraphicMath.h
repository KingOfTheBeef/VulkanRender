//
// Created by jonat on 20/08/2020.
//

#ifndef DYNAMICLINK_GRAPHICMATH_H
#define DYNAMICLINK_GRAPHICMATH_H

// Matrices in COLUMN-MAJOR order

namespace GMATH {

    struct mat4 {
        float value[16];
    };

    static mat4 identityMatrix() {
        mat4 matrix = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
        return matrix;
    }

    // Orthographic matrix for Vulkan NDC
    static mat4 orthographicMatrix(float left, float right, float top, float bottom, float close, float distant) {
        mat4 matrix = {
                2.0f / (right - left), 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / (bottom - top), 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f / (distant - close), 0.0f,
                -1.0f * (right + left) / (right - left), -1.0f * (top + bottom) / (bottom - top),-1.0f * close / (distant - close), 1.0f
        };
        return matrix;
    }
}

#endif //DYNAMICLINK_GRAPHICMATH_H
