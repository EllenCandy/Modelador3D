#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#define largura 1100
#define altura 600

#include <vector>
#include "math.hpp"
#include "scene.hpp"


struct CameraConfig {
    Vec3 VRP;
    Vec3 P;
    Vec3 Y_up;
    float xmin, xmax, ymin, ymax;
    float umin, umax, vmin, vmax;
    float Near, Far;
    float DP;
    float Cu, Cv;
};

struct Light {
    Vec3 posicao;
    float Ia, Id, Is;
};

Matriz4x4 MatrizA(CameraConfig c);
Matriz4x4 MatrizB(CameraConfig c);
Matriz4x4 MatrizC(CameraConfig c);
Matriz4x4 MatrizD(CameraConfig c);
Matriz4x4 MatrizN(CameraConfig c);
Matriz4x4 MatrizP(CameraConfig c);
Matriz4x4 MatrizK(void);
Matriz4x4 MatrizL(CameraConfig c);
Matriz4x4 MatrizM(void);
Matriz4x4 MatrizJ(void);
Matriz4x4 MatrizS(CameraConfig c);

void rasterizaTriangulo( 
    Vec4 a, Vec4 b, Vec4 c, 
    unsigned char framebuffer[][largura][3], 
    float zBuffer[][largura], 
    int W, int H,
    unsigned char corR, unsigned char corG, unsigned char corB 
);

void calcularIluminacao(
    Vec3 normal,           
    Vec3 posicaoFace,      
    Light luz,             
    Material material,     
    Vec3 posicaoCamera,    
    unsigned char& r,      
    unsigned char& g,      
    unsigned char& b       
);

std::vector<Vec4> cliparPoligono(const std::vector<Vec4>& poligono);

struct CuboTransformado;

Matriz4x4 criaMatrizProjecao(CameraConfig c);
std::vector<CuboTransformado> pipeline(CameraConfig c, Cena cena);

#endif
