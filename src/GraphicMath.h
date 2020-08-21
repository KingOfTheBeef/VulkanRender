//
// Created by jonat on 20/08/2020.
//

#ifndef DYNAMICLINK_GRAPHICMATH_H
#define DYNAMICLINK_GRAPHICMATH_H

// Matrices in COLUMN-MAJOR order

struct mat4 {
    float value[16];
};

static mat4 identityMatrix() {
    mat4 matrix = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f,0.0f,0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    };
    return matrix;
}

static mat4 orthographicMatrix(float left, float right, float top, float bottom, float near, float far) {
    mat4 matrix = {
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom),0.0f,0.0f,
            0.0f, 0.0f, -2.0f / (far - near), 0.0f,
            -1.0f * (right + left) / (right - left), -1.0f * (top + bottom) / (top - bottom), -1.0f * (far + near) / (far - near), 1.0f
    };
    return matrix;
}

#endif //DYNAMICLINK_GRAPHICMATH_H
