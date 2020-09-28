//
// Created by jonat on 20/08/2020.
//

#ifndef DYNAMICLINK_GRAPHICMATH_H
#define DYNAMICLINK_GRAPHICMATH_H

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Matrices in COLUMN-MAJOR order

namespace GMATH {
    // Abstract away from glm types
    typedef glm::mat4 mat4;
    typedef glm::vec4 vec4;
    typedef glm::vec3 vec3;

    static vec4 quatMult(vec4 q, vec4 p) {
        return vec4(p.x * q.x - glm::dot(vec3(p.yzw), vec3(q.yzw)), p.x * q.yzw + q.x * p.yzw + glm::cross(vec3(p.yzw), vec3(q.yzw)));
    }

    static vec4 normalise(vec4 vector) {
        return glm::normalize(vector);
    }

    static vec4 lerp(vec4 a, vec4 b, float t) {
        return (1-t) * a + t * b;
    }

    static vec4 quatSlerp() {
        return vec4(0);
    }

    static mat4 identityMatrix() {
        return {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
        };
    }

    static mat4 translateMatrix(vec3 translation, mat4 matrix = identityMatrix()) {
        return glm::translate(matrix, translation);
    }

    static mat4 rotateMatrix(vec3 vector, float angle, mat4 matrix = identityMatrix()) {
        return glm::rotate(matrix, angle, vector);
    }

    // Orthographic matrix for Vulkan NDC
    static mat4 orthographicMatrix(float left, float right, float top, float bottom, float close, float distant) {
        return {
                2.0f / (right - left), 0.0f, 0.0f, 0.0f,
                0.0f, 2.0f / (bottom - top), 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f / (distant - close), 0.0f,
                -1.0f * (right + left) / (right - left), -1.0f * (top + bottom) / (bottom - top),-1.0f * close / (distant - close), 1.0f
        };
    }
}

#endif //DYNAMICLINK_GRAPHICMATH_H
