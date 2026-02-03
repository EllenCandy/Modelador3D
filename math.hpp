#ifndef MATH_HPP
#define MATH_HPP

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float x, y, z, h;
};


/*
struct Material {
    float Ka; //luz ambiente
    float Kd; //reflexão difusa
    float Ks; //reflexão especular
    float m;  //brilho (m)
    float r, g, b;
};
*/

struct Matriz4x4 {
    float m[4][4];
};

Matriz4x4 identidade();
Matriz4x4 multiplica(Matriz4x4 a, Matriz4x4 b);
Vec4 multVetor(Matriz4x4 m, Vec4 v);

Matriz4x4 translacao(float tx, float ty, float tz);
Matriz4x4 rotacaoX(float angle);
Matriz4x4 rotacaoY(float angle);
Matriz4x4 rotacaoZ(float angle);
Matriz4x4 escala(float s); 

#endif