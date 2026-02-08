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
    float R, G, B;  //cor do material
};

struct Objeto {
    Cubo* mesh;       //aponta pro cubo base
    Matriz4x4 model;  //matriz de transformação
    Material material;
};

struct Cena {
    std::vector<Objeto> objetos;
};

Cena criaCena();

#endif
