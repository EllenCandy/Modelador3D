#ifndef CUBO_HPP
#define CUBO_HPP

#include "math.hpp"
#include <vector>

struct Material; // referencia para o tipo material definido em scene.hpp

struct Face {
    int v1, v2, v3;   // índices dos vértices
    Vec3 normal;     // normal da face
};

struct Cubo {
    std::vector<Vec4> vertices; 
    std::vector<Face> faces;
};

struct CuboTransformado {
    std::vector<Vec4> vertices_clip;  // Vértices em espaço de clipping
    std::vector<Face> faces;
    Material* material;  // O ponteiro serve para evitar problemas de ordem de declaração e uso 
};

Cubo criaCubo();

#endif
