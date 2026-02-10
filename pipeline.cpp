#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include "pipeline.hpp"
#include "math.hpp"
#include "scene.hpp"
#include "cubo.hpp"

#define largura 1100
#define altura 600
#define largura_comandos 300
using namespace std;

// calcula em que lado o ponto C esta da aresta AB
static float edgeFunction(float ax, float ay, float bx, float by, float cx, float cy){ 
    return (cx - ax) * (by - ay) - (cy - ay) * (bx - ax); // POSITIVO (a esquerda) NEGATIVO (a direita)
}

void rasterizaTriangulo(
    Vec4 a, Vec4 b, Vec4 c,
    unsigned char framebuffer[][largura][3],
    float zBuffer[][largura],
    int W, int H,
    unsigned char corR, unsigned char corG, unsigned char corB)
{
    float minX = std::min(a.x, std::min(b.x, c.x));
    float maxX = std::max(a.x, std::max(b.x, c.x));
    float minY = std::min(a.y, std::min(b.y, c.y));
    float maxY = std::max(a.y, std::max(b.y, c.y));

    int xmin = std::max(0, (int)std::floor(minX));
    int xmax = std::min(W - 1, (int)std::ceil (maxX));
    int ymin = std::max(0, (int)std::floor(minY));
    int ymax = std::min(H - 1, (int)std::ceil (maxY));

    float area = edgeFunction(a.x, a.y, b.x, b.y, c.x, c.y); // verifica se o ponto esta dentro do triangulo
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

                    framebuffer[y][x + largura_comandos][0] = corR;  
                    framebuffer[y][x + largura_comandos][1] = corG;  
                    framebuffer[y][x + largura_comandos][2] = corB;  
                }
            }
        }
    }
}

// Translação do VRP para a origem			
Matriz4x4 MatrizA(CameraConfig c){
    Matriz4x4 A = identidade();
    A.m[0][3] = -c.VRP.x;
    A.m[1][3] = -c.VRP.y;
    A.m[2][3] = -c.VRP.z;
    
    return A;
}
// Matriz de mudança de base (SRU -> SRC)			
Matriz4x4 MatrizB(CameraConfig c){

    Vec3 n = normalize(sub(c.VRP, c.P));
    Vec3 u = normalize(cross(c.Y_up, n));
    Vec3 v = cross(n, u);
    Matriz4x4 B = identidade();
    
    B.m[0][0] = u.x; B.m[0][1] = u.y; B.m[0][2] = u.z;
    B.m[1][0] = v.x; B.m[1][1] = v.y; B.m[1][2] = v.z;
    B.m[2][0] = n.x; B.m[2][1] = n.y; B.m[2][2] = n.z;

    return B;
}
// Cisalhamento			
Matriz4x4 MatrizC(CameraConfig c){

    Matriz4x4 C = identidade();
    C.m[0][2] = -c.Cu/c.DP;
    C.m[1][2] = -c.Cv/c.DP;

    return C;
}
// Padroniza o tronco (frustum) com uma escala			
Matriz4x4 MatrizD(CameraConfig c){

    Matriz4x4 D = identidade();
    D.m[0][0] = c.DP / (c.xmax * c.Far);

    D.m[1][1] = c.DP / (c.ymax * c.Far);

    D.m[2][2] = 1.f / c.Far;

    return D;
}
// D*C*B*A			
Matriz4x4 MatrizN(CameraConfig c){

    Matriz4x4 N = identidade();

    Matriz4x4 D = MatrizD(c);
    Matriz4x4 C = MatrizC(c);
    Matriz4x4 B = MatrizB(c);
    Matriz4x4 A = MatrizA(c);
    

    N = multiplica(multiplica(multiplica(D, C), B), A);

    return N;
}
// Transforma o tronco em um paralelepipedo			
Matriz4x4 MatrizP(CameraConfig c){

    float zMin = c.Near / c.Far;

    Matriz4x4 P = identidade();
    P.m[2][2] = c.Far / (c.Far - c.Near);
    P.m[2][3] = -c.Near / (c.Far - c.Near); 
    P.m[3][2] = 1.0f;
    P.m[3][3] = 0.0f;

    return P;
}
// Transformação para o Core standard NDC			
Matriz4x4 MatrizK(void){
    Matriz4x4 K = identidade();
    K.m[0][0] = 0.5;
    K.m[1][1] = 0.5;
    K.m[0][3] = 0.5;  
    K.m[1][3] = 0.5;  

    return K;
}
// Mapeamento para o dispositivo de saída			 
Matriz4x4 MatrizL(CameraConfig c){
    Matriz4x4 L = identidade();
    L.m[0][0] = c.umax - c.umin;
    L.m[1][1] = c.vmax - c.vmin;
    L.m[3][0] = c.umin;
    L.m[3][1] = c.vmin;

    return L;
}
// Matriz de arredondamento (float para pixel)			
Matriz4x4 MatrizM(void){
    Matriz4x4 M = identidade();
    M.m[0][3] = 0.5;
    M.m[1][3] = 0.5;

    return M;
}
// Inverte o Y para converter o espaço NDC para as coordenadas de tela			
Matriz4x4 MatrizJ(void){
    Matriz4x4 J = identidade();
    J.m[1][1] = -1;

    return J;
}
// M*L*K*J			
Matriz4x4 MatrizS(CameraConfig c){
    Matriz4x4 S = identidade();
    Matriz4x4 M = MatrizM();
    Matriz4x4 L = MatrizL(c);
    Matriz4x4 K = MatrizK();
    Matriz4x4 J = MatrizJ();
    S = multiplica(multiplica(multiplica(M, L), K), J);

    return S;
}

std::vector<Vec4> aplicaMatrizVertices(const Matriz4x4& mat, const std::vector<Vec4>& vertices) {
    std::vector<Vec4> resultado;
    resultado.reserve(vertices.size());
    
    for (const Vec4& v : vertices) {
        resultado.push_back(multVetor(mat, v));
    }
    
    return resultado;
}


Matriz4x4 criaMatrizProjecao(CameraConfig c) {
    Matriz4x4 P = {0};

    float aspect = (c.xmax - c.xmin) / (c.ymax - c.ymin);
    float fov = 2.0f * atan(c.ymax / c.Near);
    float f = 1.0f / tan(fov / 2.0f);
    
    P.m[0][0] = f / aspect;
    P.m[1][1] = f;
    P.m[2][2] = (c.Far + c.Near) / (c.Near - c.Far);
    P.m[2][3] = (2.0f * c.Far * c.Near) / (c.Near - c.Far);
    P.m[3][2] = -1.0f;
    P.m[3][3] = 0.0f;
    
    return P;
}



// Serve para atualizar
std::vector<CuboTransformado> pipeline(CameraConfig c, Cena cena) {
    std::vector<CuboTransformado> resultados;
    Matriz4x4 View = multiplica(MatrizB(c), MatrizA(c));
    Matriz4x4 Proj = criaMatrizProjecao(c);
    Matriz4x4 VP = multiplica(Proj, View);
    
    for (const Objeto& obj : cena.objetos) {
        CuboTransformado ct;
        ct.faces = obj.mesh->faces;
        ct.material = const_cast<Material*>(&obj.material);
        
        Matriz4x4 MVP = multiplica(VP, obj.model);
        ct.vertices_clip = aplicaMatrizVertices(MVP, obj.mesh->vertices);
        resultados.push_back(ct);
    }
    return resultados;
}



enum PlanoClip {
    CLIP_LEFT, 
    CLIP_RIGHT, 
    CLIP_BOTTOM,
    CLIP_TOP, 
    CLIP_NEAR, 
    CLIP_FAR
};

bool dentroDoPlano(const Vec4& v, PlanoClip plano) {
    const float EPSILON = 0.0001f;
    switch(plano) {
        case CLIP_LEFT:   return v.x >= -v.h - EPSILON;
        case CLIP_RIGHT:  return v.x <= v.h + EPSILON;
        case CLIP_BOTTOM: return v.y >= -v.h - EPSILON;
        case CLIP_TOP:    return v.y <= v.h + EPSILON;
        case CLIP_NEAR:   return v.z >= -EPSILON;
        case CLIP_FAR:    return v.z <= v.h + EPSILON;
    }
    return true;
}

Vec4 interseccaoPlano(const Vec4& v1, const Vec4& v2, PlanoClip plano) {
    float t = 0.0f;
    
    switch(plano) {
        case CLIP_LEFT: {
            float num = v1.x + v1.h;
            float den = (v1.x + v1.h) - (v2.x + v2.h);
            t = (fabs(den) > 0.0001f) ? num / den : 0.0f;
            break;
        }
        case CLIP_RIGHT: {
            float num = v1.h - v1.x;
            float den = (v1.h - v1.x) - (v2.h - v2.x);
            t = (fabs(den) > 0.0001f) ? num / den : 0.0f;
            break;
        }
        case CLIP_BOTTOM: {
            float num = v1.y + v1.h;
            float den = (v1.y + v1.h) - (v2.y + v2.h);
            t = (fabs(den) > 0.0001f) ? num / den : 0.0f;
            break;
        }
        case CLIP_TOP: {
            float num = v1.h - v1.y;
            float den = (v1.h - v1.y) - (v2.h - v2.y);
            t = (fabs(den) > 0.0001f) ? num / den : 0.0f;
            break;
        }
        case CLIP_NEAR: {
            float num = v1.z;
            float den = v1.z - v2.z;
            t = (fabs(den) > 0.0001f) ? num / den : 0.0f;
            break;
        }
        case CLIP_FAR: {
            float num = v1.h - v1.z;
            float den = (v1.h - v1.z) - (v2.h - v2.z);
            t = (fabs(den) > 0.0001f) ? num / den : 0.0f;
            break;
        }
    }
    
    t = std::max(0.0f, std::min(1.0f, t));
    
    Vec4 resultado;
    resultado.x = v1.x + t * (v2.x - v1.x);
    resultado.y = v1.y + t * (v2.y - v1.y);
    resultado.z = v1.z + t * (v2.z - v1.z);
    resultado.h = v1.h + t * (v2.h - v1.h);
    
    return resultado;
}

std::vector<Vec4> cliparContraPlano(const std::vector<Vec4>& entrada, PlanoClip plano) {
    std::vector<Vec4> saida;
    
    if (entrada.size() < 3) return saida;
    
    for (size_t i = 0; i < entrada.size(); i++) {
        const Vec4& atual = entrada[i];
        const Vec4& proximo = entrada[(i + 1) % entrada.size()];
        
        bool atualDentro = dentroDoPlano(atual, plano);
        bool proximoDentro = dentroDoPlano(proximo, plano);
        
        if (atualDentro) {
            saida.push_back(atual);
            
            if (!proximoDentro) {
                saida.push_back(interseccaoPlano(atual, proximo, plano));
            }
        } else if (proximoDentro) {
            saida.push_back(interseccaoPlano(atual, proximo, plano));
        }
    }
    
    return saida;
}

std::vector<Vec4> cliparPoligono(const std::vector<Vec4>& poligono) {
    std::vector<Vec4> resultado = poligono;
    
    PlanoClip planos[] = {
        CLIP_LEFT, CLIP_RIGHT, 
        CLIP_BOTTOM, CLIP_TOP,
        CLIP_NEAR, CLIP_FAR
    };
    
    for (int i = 0; i < 6; i++) {
        resultado = cliparContraPlano(resultado, planos[i]);
        if (resultado.size() < 3) break;
    }
    
    return resultado;
}

void calcularIluminacao(
    Vec3 normal,
    Vec3 posicaoFace,
    Light luz,
    Material material,
    Vec3 posicaoCamera,
    unsigned char& r,
    unsigned char& g,
    unsigned char& b)
{
    // Normalizar a normal
    normal = normalize(normal);
    
    // Componente ambiente
    float ambient = luz.Ia * material.Ka;
    
    // Direção da luz
    Vec3 L = normalize(sub(luz.posicao, posicaoFace));
    
    // Componente difusa
    float diff = std::max(0.0f, dot(normal, L));
    float diffuse = luz.Id * material.Kd * diff;
    
    // Direção da visão
    Vec3 V = normalize(sub(posicaoCamera, posicaoFace));
    
    // Direção refletida
    Vec3 R = normalize(sub(L, {2.0f * dot(L, normal) * normal.x,
                                2.0f * dot(L, normal) * normal.y,
                                2.0f * dot(L, normal) * normal.z}));
    
    // Componente especular
    float spec = std::pow(std::max(0.0f, dot(R, V)), material.m);
    float specular = luz.Is * material.Ks * spec;
    
    // Intensidade total
    float intensity = ambient + diffuse + specular;
    intensity = std::min(1.0f, std::max(0.0f, intensity));
    
    // Aplicar à cor do material
    r = (unsigned char)(intensity * material.r * 255);
    g = (unsigned char)(intensity * material.g * 255);
    b = (unsigned char)(intensity * material.b * 255);
}

