#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "pipeline.hpp"

using namespace std;

/*
struct Matriz4x4 {
    double m[4][4] = {0};
    
    static Matriz4x4 identidade() {
        Matriz4x4 res;
        for(int i=0; i<4; i++) res.m[i][i] = 1.0;
        return res;
    }
};
*/

//funções matematicas de vetores
Vec3 subtract(Vec3 a, Vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
float length(Vec3 v) { return sqrt(v.x*v.x + v.y*v.y + v.z*v.z); }
Vec3 normalize(Vec3 v) {
    float len = length(v);
    return {v.x/len, v.y/len, v.z/len};
}
Vec3 cross(Vec3 a, Vec3 b) {
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}

//multiplicacao de matrizes (R = A * B)
Matriz4x4 multiply(Matriz4x4 a, Matriz4x4 b) {
    Matriz4x4 res;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                res.m[i][j] += a.m[i][k] * b.m[k][j];
    return res;
}

// Transformação de Ponto por Matriz
void transformPoint(Matriz4x4 m, float pin[4], float pout[4]) {
    for (int i = 0; i < 4; i++) {
        pout[i] = 0;
        for (int j = 0; j < 4; j++)
            pout[i] += m.m[i][j] * pin[j];
    }
    // Normalização Homogênea (W)
    if (pout[3] != 0 && pout[3] != 1.0) {
        pout[0] /= pout[3];
        pout[1] /= pout[3];
        pout[2] /= pout[3];
    }
}

int pipeline() {
    // 1. Parâmetros da Planilha
    Vec3 VRP = {30, 40, 100};
    Vec3 P = {1, 2, 1};
    Vec3 Y_up = {0, 1, 0};
    
    double xmin = -10, xmax = 10, ymin = -8, ymax = 8;
    double umin = 100, umax = 1000, vmin = 300, vmax = 900;
    double Near = 20, Far = 160;

    Vec3 n = normalize(subtract(VRP, P));
    Vec3 u = normalize(cross(Y_up, n));
    Vec3 v = cross(n, u);

    // Matriz A
    Matriz4x4 A = identidade();
    A.m[0][3] = -VRP.x;
    A.m[1][3] = -VRP.y;
    A.m[2][3] = -VRP.z;

    // Matriz B
    Matriz4x4 B = identidade();
    B.m[0][0] = u.x; B.m[0][1] = u.y; B.m[0][2] = u.z;
    B.m[1][0] = v.x; B.m[1][1] = v.y; B.m[1][2] = v.z;
    B.m[2][0] = n.x; B.m[2][1] = n.y; B.m[2][2] = n.z;

    // Matriz P
    Matriz4x4 P_mat = identidade();
    P_mat.m[2][2] = Far / (Far - Near);
    P_mat.m[2][3] = -(Far * Near) / (Far - Near);
    P_mat.m[3][2] = 1.0; 
    P_mat.m[3][3] = 0.0;

    // Matriz S
    Matriz4x4 S = identidade();
    double scaleX = (umax - umin) / (xmax - xmin);
    double scaleY = (vmax - vmin) / (ymax - ymin);
    S.m[0][0] = scaleX;
    S.m[0][3] = umin - (scaleX * xmin);
    S.m[1][1] = -scaleY; // Inversão Y 
    S.m[1][3] = vmax + (scaleY * ymin);

    Matriz4x4 finalMatriz = multiply(S, multiply(P_mat, multiply(B, A)));

    // pontos para Transformar
    float pontos[5][4] = {
        {-2, -1, 4, 1},  // A
        {3, -2, 5, 1},   // B
        {4, -1, -2, 1},  // C
        {-1, 0, -3, 1},  // D
        {1, 6, 1, 1}     // E
    };

    cout << fixed << setprecision(4);
    cout << " Resultado da transformação " << endl;
    char labels[] = {'A', 'B', 'C', 'D', 'E'};
    for(int i=0; i<5; i++) {
        float out[4];
        transformPoint(finalMatriz, pontos[i], out);
        cout << labels[i] << ": (" << out[0] << ", " << out[1] << ", " << out[2] << ")" << endl;
    }

    return 0;
}