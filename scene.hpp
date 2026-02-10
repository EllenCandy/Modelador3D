#ifndef SCENE_HPP
#define SCENE_HPP

#include "math.hpp"
#include "cubo.hpp"
#include <vector>

struct Material {
    float Ka; //luz ambiente
    float Kd; //reflexão difusa
    float Ks; //reflexão especular
    float m;  //brilho (m)
    float r, g, b;
};

struct Objeto {
    Cubo* mesh;
    Matriz4x4 model;
    Material material;
};

struct Cena {
    std::vector<Objeto> objetos;
};

Cena criaCena();

#endif
