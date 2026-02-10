#ifndef MATH_HPP
#define MATH_HPP

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float x, y, z, h;
};


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
Matriz4x4 escala(float sx, float sy, float sz);

// Vec3 add(Vec3 a, Vec3 b);
Vec3 sub(Vec3 a, Vec3 b);
float dot(Vec3 a, Vec3 b);
Vec3 cross(Vec3 a, Vec3 b);
float length(Vec3 v);
Vec3 normalize(Vec3 v);

#endif