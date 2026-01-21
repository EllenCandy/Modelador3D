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

Matriz4x4 escala(float sx, float sy, float sz) {
    Matriz4x4 res = identidade();
    res.m[0][0] = sx; res.m[1][1] = sy; res.m[2][2] = sz;
    return res;
}