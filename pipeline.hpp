#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#define largura 800
#define altura 600

#include "math.hpp"
#include "scene.hpp" // ADIÇÃO: Necessário para reconhecer 'Material'

Matriz4x4 montaPipeline(
    Vec3 VRP, Vec3 P, Vec3 Y_up,
    float xmin, float xmax, float ymin, float ymax,
    float umin, float umax, float vmin, float vmax,
    float Near, float Far
);

struct CameraConfig {
    Vec3 VRP;
    Vec3 P;
    Vec3 Y_up;
    float xmin, xmax, ymin, ymax;
    float umin, umax, vmin, vmax;
    float Near, Far;
};

struct Light {
    Vec3 posicao;
    float Ia, Id, Is;
};

Matriz4x4 calculaMatrizPipeline(CameraConfig c);

Vec4 aplicaPipeline(Matriz4x4 M, Vec4 p);

// MUDANÇA: Renomeado r,g,b para corR, corG, corB para evitar conflito com vértice 'b'
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

#endif