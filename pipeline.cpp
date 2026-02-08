#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "pipeline.hpp"
#include "math.hpp"

#define largura 1920
#define altura 1079

using namespace std;

static float edgeFunction(float ax, float ay, float bx, float by, float cx, float cy){ // produto vetorial
    return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
}

void rasterizaTriangulo(
    Vec4 a, Vec4 b, Vec4 c,
    unsigned char framebuffer[][largura][3],
    float zBuffer[][largura],
    int W, int H)
{
    // Bounding box
    float minX = std::min(a.x, std::min(b.x, c.x));
    float maxX = std::max(a.x, std::max(b.x, c.x));
    float minY = std::min(a.y, std::min(b.y, c.y));
    float maxY = std::max(a.y, std::max(b.y, c.y));

    int xmin = std::max(0, (int)std::floor(minX));
    int xmax = std::min(W - 1, (int)std::ceil (maxX));
    int ymin = std::max(0, (int)std::floor(minY));
    int ymax = std::min(H - 1, (int)std::ceil (maxY));

    // Área total do triângulo
    float area = edgeFunction(a.x, a.y, b.x, b.y, c.x, c.y);
    if (area == 0) return; // degenerado

    // Rasterização
    for (int y = ymin; y <= ymax; y++) {
        for (int x = xmin; x <= xmax; x++) {

            float w0 = edgeFunction(b.x, b.y, c.x, c.y, x + 0.5f, y + 0.5f);
            float w1 = edgeFunction(c.x, c.y, a.x, a.y, x + 0.5f, y + 0.5f);
            float w2 = edgeFunction(a.x, a.y, b.x, b.y, x + 0.5f, y + 0.5f);

            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                float b0 = w0 / area;
                float b1 = w1 / area;
                float b2 = w2 / area;

                float z = b0 * a.z + b1 * b.z + b2 * c.z;

                if (z < zBuffer[y][x]) {
                    zBuffer[y][x] = z;

                    framebuffer[y][x][0] = 255;
                    framebuffer[y][x][1] = 255;
                    framebuffer[y][x][2] = 255;
                }
            }
        }
    }
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

Matriz4x4 calculaMatrizPipeline(CameraConfig c) {

    Vec3 n = normalize(sub(c.VRP, c.P));
    Vec3 u = normalize(cross(c.Y_up, n));
    Vec3 v = cross(n, u);

    // Matriz A
    Matriz4x4 A = identidade();
    A.m[0][3] = -c.VRP.x;
    A.m[1][3] = -c.VRP.y;
    A.m[2][3] = -c.VRP.z;

    // Matriz B
    Matriz4x4 B = identidade();
    B.m[0][0] = u.x; B.m[0][1] = u.y; B.m[0][2] = u.z;
    B.m[1][0] = v.x; B.m[1][1] = v.y; B.m[1][2] = v.z;
    B.m[2][0] = n.x; B.m[2][1] = n.y; B.m[2][2] = n.z;

    // Matriz P
    Matriz4x4 P_mat = identidade();
    P_mat.m[2][2] = c.Far / (c.Far - c.Near);
    P_mat.m[2][3] = -(c.Far * c.Near) / (c.Far - c.Near);
    P_mat.m[3][2] = 1.0; 
    P_mat.m[3][3] = 0.0;

    // Matriz S
    Matriz4x4 S = identidade();
    float scaleX = (c.umax - c.umin) / (c.xmax - c.xmin);
    float scaleY = (c.vmax - c.vmin) / (c.ymax - c.ymin);
    S.m[0][0] = scaleX;
    S.m[0][3] = c.umin - (scaleX * c.xmin);
    S.m[1][1] = -scaleY; // Inversão Y 
    S.m[1][3] = c.vmax + (scaleY * c.ymin);

    return multiply(S, multiply(P_mat, multiply(B, A)));
}

Matriz4x4 calculaMatrizPipeline(CameraConfig c);

Vec4 aplicaPipeline(Matriz4x4 M, Vec4 p) {
    Vec4 r = multVetor(M, p);
    if (r.h != 0) {
        r.x /= r.h;
        r.y /= r.h;
        r.z /= r.h;
    }
    return r;
}