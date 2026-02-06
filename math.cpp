#include "math.hpp"
#include <cmath>

Matriz4x4 identidade() {
    Matriz4x4 res = {0};
    for(int i=0; i<4; i++) res.m[i][i] = 1.0f;
    return res;
}

Matriz4x4 multiplica(Matriz4x4 a, Matriz4x4 b) {
    Matriz4x4 res = {0};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                res.m[i][j] += a.m[i][k] * b.m[k][j];
    return res;
}

Vec4 multVetor(Matriz4x4 m, Vec4 v) {
    return {
        m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]*v.h,
        m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]*v.h,
        m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]*v.h,
        m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]*v.h
    };
}

Matriz4x4 translacao(float tx, float ty, float tz) {
    Matriz4x4 res = identidade();
    res.m[0][3] = tx; res.m[1][3] = ty; res.m[2][3] = tz;
    return res;
}

Matriz4x4 rotacaoX(float angle) {
    Matriz4x4 res = identidade();
    float c = cos(angle);
    float s = sin(angle);

    res.m[1][1] = c;
    res.m[1][2] = -s;
    res.m[2][1] = s;
    res.m[2][2] = c;

    return res;
}

Matriz4x4 rotacaoY(float angle) {
    Matriz4x4 res = identidade();
    float c = cos(angle);
    float s = sin(angle);

    res.m[0][0] = c;
    res.m[0][2] = s;
    res.m[2][0] = -s;
    res.m[2][2] = c;

    return res;
}

Matriz4x4 rotacaoZ(float angle) {
    Matriz4x4 res = identidade();
    float c = cos(angle);
    float s = sin(angle);

    res.m[0][0] = c;
    res.m[0][1] = -s;
    res.m[1][0] = s;
    res.m[1][1] = c;

    return res;
}

Matriz4x4 escala(float sx, float sy, float sz) {
    Matriz4x4 res = identidade();
    res.m[0][0] = sx; res.m[1][1] = sy; res.m[2][2] = sz;
    return res;
}

Vec3 add(Vec3 a, Vec3 b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Vec3 sub(Vec3 a, Vec3 b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

float dot(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vec3 cross(Vec3 a, Vec3 b) {
    return {
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    };
}

float length(Vec3 v) {
    return std::sqrt(dot(v, v));
}

Vec3 normalize(Vec3 v) {
    float len = length(v);
    if (len == 0) return {0, 0, 0};
    return { v.x/len, v.y/len, v.z/len };
}
