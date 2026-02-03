#include "cubo.hpp"

Cubo criaCubo() {
    Cubo c;

    // 8 vértices do cubo (coordenadas de objeto)
    c.vertices = {
        {-1, -1, -1, 1}, // 0
        { 1, -1, -1, 1}, // 1
        { 1,  1, -1, 1}, // 2
        {-1,  1, -1, 1}, // 3
        {-1, -1,  1, 1}, // 4
        { 1, -1,  1, 1}, // 5
        { 1,  1,  1, 1}, // 6
        {-1,  1,  1, 1}  // 7
    };

    // 12 faces (triângulos)
    c.faces = {
        {0,1,2, {0,0,-1}}, {0,2,3, {0,0,-1}}, // frente
        {4,6,5, {0,0, 1}}, {4,7,6, {0,0, 1}}, // trás
        {0,3,7, {-1,0,0}}, {0,7,4, {-1,0,0}}, // esquerda
        {1,5,6, { 1,0,0}}, {1,6,2, { 1,0,0}}, // direita
        {3,2,6, {0,1,0}},  {3,6,7, {0,1,0}},  // cima
        {0,4,5, {0,-1,0}}, {0,5,1, {0,-1,0}}  // baixo
    };

    return c;
}

/*
ANOTAÇÃO PRA ELLEN FAZER ESSA PORCARIA

vertices:
0 = inferior esquerdo frontal
1 = inferior direito frontal
2 = superior direito frontal
3 = superior esquerdo frontal
4 = inferior esquerdo traseiro
5 = inferior direito traseiro
6 = superior direito traseiro
7 = superior esquerdo traseiro

normais:
{0, 0, -1} = Face Frontal
{0, 0, 1} = Face Traseira
{-1, 0, 0} = Face Esquerda
{1, 0, 0} = Face Direita
{0, 1, 0} = Face Superior
{0, -1, 0} = Face Inferior

*/