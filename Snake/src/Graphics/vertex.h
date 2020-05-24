#pragma once

#include <glm/glm.hpp>

struct vertex {
    glm::vec4 pos;
    glm::vec2 tex;
    glm::vec2 temp;

    vertex(float x, float y, float z, float w, float tx = 0.0f, float ty = 0.0f) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        pos.w = w;

        tex.x = tx;
        tex.y = ty;

        temp.x = 0.0f;
        temp.y = 0.0f;
    }
};