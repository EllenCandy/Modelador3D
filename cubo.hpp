#ifndef CUBO_HPP
#define CUBO_HPP

#include "math.hpp"
#include <vector>

struct Face {
    int v1, v2, v3;   // índices dos vértices
    Vec3 normal;     // normal da face
};

struct Cubo {
    std::vector<Vec4> vertices; 
    std::vector<Face> faces;
};

Cubo criaCubo();

#endif
