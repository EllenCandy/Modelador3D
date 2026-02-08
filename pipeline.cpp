#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "pipeline.hpp"
#include "math.hpp"
#include "scene.hpp"  // para ter acesso à struct Material

#define largura 800
#define altura 600

using namespace std;

static float edgeFunction(float ax, float ay, float bx, float by, float cx, float cy){ 
    return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax);
}

// ============================================================
// MODIFICAÇÃO 1: rasterizaTriangulo agora recebe cor RGB
// ============================================================
// MOTIVAÇÃO: No sombreamento constante, a cor é uniforme para toda
// a face. Calculamos a iluminação UMA VEZ (no main.cpp) e passamos
// a cor final para esta função.
//
// MUDANÇAS:
// - Adicionados parâmetros r, g, b
// - Substituído hardcode (255,255,255) por (r,g,b)
void rasterizaTriangulo(
    Vec4 a, Vec4 b, Vec4 c,
    unsigned char framebuffer[][largura][3],
    float zBuffer[][largura],
    int W, int H,
    unsigned char corR, unsigned char corG, unsigned char corB)  // MUDANÇA AQUI
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

    float area = edgeFunction(a.x, a.y, b.x, b.y, c.x, c.y);
    if (area == 0) return;

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

                    // MUDANÇA: Usando os novos nomes
                    framebuffer[y][x][0] = corR;  
                    framebuffer[y][x][1] = corG;  
                    framebuffer[y][x][2] = corB;  
                }
            }
        }
    }
}

Matriz4x4 multiply(Matriz4x4 a, Matriz4x4 b) {
    Matriz4x4 res = {0};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                res.m[i][j] += a.m[i][k] * b.m[k][j];
    return res;
}

void transformPoint(Matriz4x4 m, float pin[4], float pout[4]) {
    for (int i = 0; i < 4; i++) {
        pout[i] = 0;
        for (int j = 0; j < 4; j++)
            pout[i] += m.m[i][j] * pin[j];
    }
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

    Matriz4x4 A = identidade();
    A.m[0][3] = -c.VRP.x;
    A.m[1][3] = -c.VRP.y;
    A.m[2][3] = -c.VRP.z;

    Matriz4x4 B = identidade();
    B.m[0][0] = u.x; B.m[0][1] = u.y; B.m[0][2] = u.z;
    B.m[1][0] = v.x; B.m[1][1] = v.y; B.m[1][2] = v.z;
    B.m[2][0] = n.x; B.m[2][1] = n.y; B.m[2][2] = n.z;

    Matriz4x4 P_mat = identidade();
    P_mat.m[2][2] = c.Far / (c.Far - c.Near);
    P_mat.m[2][3] = -(c.Far * c.Near) / (c.Far - c.Near);
    P_mat.m[3][2] = 1.0; 
    P_mat.m[3][3] = 0.0;

    Matriz4x4 S = identidade();
    float scaleX = (c.umax - c.umin) / (c.xmax - c.xmin);
    float scaleY = (c.vmax - c.vmin) / (c.ymax - c.ymin);
    S.m[0][0] = scaleX;
    S.m[0][3] = c.umin - (scaleX * c.xmin);
    S.m[1][1] = -scaleY;
    S.m[1][3] = c.vmax + (scaleY * c.ymin);

    return multiply(S, multiply(P_mat, multiply(B, A)));
}

Vec4 aplicaPipeline(Matriz4x4 M, Vec4 p) {
    Vec4 r = multVetor(M, p);
    if (r.h != 0) {
        r.x /= r.h;
        r.y /= r.h;
        r.z /= r.h;
    }
    return r;
}

// ============================================================
// ADIÇÃO: Função calcularIluminacao
// ============================================================
// MOTIVAÇÃO: Implementar o modelo de iluminação de Phong para
// sombreamento constante (flat shading).
//
// ALGORITMO:
// 1. Normalizar a normal da face
// 2. Calcular vetor L (da face para a luz)
// 3. Calcular componente ambiente: Iamb = Ia * Ka
// 4. Calcular componente difusa: Idif = Id * Kd * max(N·L, 0)
// 5. Calcular vetor R (reflexão da luz)
// 6. Calcular vetor V (da face para a câmera)
// 7. Calcular componente especular: Iesp = Is * Ks * max(R·V, 0)^m
// 8. Somar componentes e multiplicar pela cor do material
// 9. Clampar para [0, 255]
//
// IMPORTANTE: Todos os vetores devem estar no ESPAÇO DA CÂMERA!
void calcularIluminacao(
    Vec3 normal,
    Vec3 posicaoFace,
    Light luz,
    Material material,
    Vec3 posicaoCamera,  // no espaço da câmera, isso é (0,0,0)
    unsigned char& R,
    unsigned char& G,
    unsigned char& B)
{
    // PASSO 1: Normalizar a normal (pode ter sido transformada)
    Vec3 N = normalize(normal);
    
    // PASSO 2: Vetor da face para a luz (L)
    Vec3 L = normalize(sub(luz.posicao, posicaoFace));
    
    // PASSO 3: Componente AMBIENTE
    // Iluminação mínima que existe mesmo sem luz direta
    float Iamb = luz.Ia * material.Ka;
    
    // PASSO 4: Componente DIFUSA (Lambertiana)
    // Depende do ângulo entre a normal e a direção da luz
    // max(N·L, 0) garante que faces opostas à luz não sejam iluminadas
    float NdotL = dot(N, L);
    float Idif = 0.0f;
    float Iesp = 0.0f;
    
    if (NdotL > 0) {  // Se a face está voltada para a luz
        Idif = luz.Id * material.Kd * NdotL;
        
        // PASSO 5-7: Componente ESPECULAR (brilho)
        // R = 2*(N·L)*N - L  (vetor de reflexão)
        Vec3 R;
        R.x = 2.0f * NdotL * N.x - L.x;
        R.y = 2.0f * NdotL * N.y - L.y;
        R.z = 2.0f * NdotL * N.z - L.z;
        R = normalize(R);
        
        // V = vetor da face para a câmera
        // No espaço da câmera, a câmera está em (0,0,0)
        Vec3 V = normalize(sub(posicaoCamera, posicaoFace));
        
        float RdotV = dot(R, V);
        if (RdotV > 0) {
            // pow(RdotV, m) cria o efeito de brilho concentrado
            // m maior = brilho mais concentrado
            Iesp = luz.Is * material.Ks * pow(RdotV, material.m);
        }
    }
    
    // PASSO 8: Intensidade total
    float I_total = Iamb + Idif + Iesp;
    
    // Multiplicar pela cor do material e clampar para [0, 255]
    float cor_r = I_total * material.R * 255.0f;
    float cor_g = I_total * material.G * 255.0f;
    float cor_b = I_total * material.B * 255.0f;
    
    // PASSO 9: Clampar (evitar overflow)
    R = (unsigned char) std::min(255.0f, std::max(0.0f, cor_r));
    G = (unsigned char) std::min(255.0f, std::max(0.0f, cor_g));
    B = (unsigned char) std::min(255.0f, std::max(0.0f, cor_b));
}
