#include <iostream>
#include <GLFW/glfw3.h>
#include "math.hpp"
#include "pipeline.hpp"
#include "cubo.hpp"
#include "scene.hpp"

#define largura 800
#define altura 600

int main() {

    if (!glfwInit()) {
        std::cout << "Erro ao iniciar GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(largura, altura, "CG - Modelador 3D", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    const int W = largura;
    const int H = altura;

    static unsigned char framebuffer[H][W][3];
    static float zBuffer[H][W];

    Cena cena = criaCena(); 
    
    CameraConfig cam;
    cam.VRP = {0, 0, 50};
    cam.P   = {0, 0, 0};
    cam.Y_up = {0, 1, 0};

    cam.Near = 10;
    cam.Far  = 200;
    
    cam.xmin = -10; cam.xmax = 10;
    cam.ymin = -8;  cam.ymax = 8;

    cam.umin = 0; cam.umax = (float)W;
    cam.vmin = 0; cam.vmax = (float)H;

    // Light luzMundo;
    // luzMundo.posicao = {20, 30, 40};
    // luzMundo.Ia = 0.3f;
    // luzMundo.Id = 0.7f;
    // luzMundo.Is = 0.8f;

    // CONFIGURAÇÃO DA LUZ (No Espaço do Mundo)
    Light luzMundo;
    // Colocar a luz na mesma posição da Câmera garante que veremos as faces iluminadas
    luzMundo.posicao = {0, 0, 50}; 
    luzMundo.Ia = 1.0f; // AUMENTADO PARA 1.0 (Luz ambiente forte para teste)
    luzMundo.Id = 1.0f; // AUMENTADO PARA 1.0
    luzMundo.Is = 1.0f; // AUMENTADO PARA 1.0

    // Cálculo da Matriz View
    Vec3 n = normalize(sub(cam.VRP, cam.P));
    Vec3 u = normalize(cross(cam.Y_up, n));
    Vec3 v = cross(n, u);

    Matriz4x4 A = identidade();
    A.m[0][3] = -cam.VRP.x; A.m[1][3] = -cam.VRP.y; A.m[2][3] = -cam.VRP.z;

    Matriz4x4 B = identidade();
    B.m[0][0] = u.x; B.m[0][1] = u.y; B.m[0][2] = u.z;
    B.m[1][0] = v.x; B.m[1][1] = v.y; B.m[1][2] = v.z;
    B.m[2][0] = n.x; B.m[2][1] = n.y; B.m[2][2] = n.z;

    Matriz4x4 viewMatriz = multiplica(B, A);

    // Transforma luz para View Space
    Vec4 luzPosMundo = {luzMundo.posicao.x, luzMundo.posicao.y, luzMundo.posicao.z, 1.0f};
    Vec4 luzPosView  = multVetor(viewMatriz, luzPosMundo);
    
    Light luzCamera = luzMundo;
    luzCamera.posicao = {luzPosView.x, luzPosView.y, luzPosView.z};


    Matriz4x4 pipelineBase = calculaMatrizPipeline(cam);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        // ============================================================
        // MUDANÇA AQUI: Limpa framebuffer com BRANCO (255)
        // ============================================================
        for(int y=0; y<H; y++) {
            for(int x=0; x<W; x++) {
                framebuffer[y][x][0] = 255; // R (Branco)
                framebuffer[y][x][1] = 255; // G (Branco)
                framebuffer[y][x][2] = 255; // B (Branco)
                zBuffer[y][x] = 9999999999.0f;
            }
        }

        for (const Objeto& obj : cena.objetos) {
            
            Matriz4x4 matEscala = escala(4.0f, 4.0f, 4.0f);
            Matriz4x4 modelFinal = multiplica(obj.model, matEscala);

            Matriz4x4 finalMatriz = multiplica(pipelineBase, modelFinal);
            Matriz4x4 viewModelMatriz = multiplica(viewMatriz, modelFinal);

            for (const Face& f : obj.mesh->faces) {

                Vec4 v0 = obj.mesh->vertices[f.v1];
                Vec4 v1 = obj.mesh->vertices[f.v2];
                Vec4 v2 = obj.mesh->vertices[f.v3];

                Vec4 p0 = aplicaPipeline(finalMatriz, v0);
                Vec4 p1 = aplicaPipeline(finalMatriz, v1);
                Vec4 p2 = aplicaPipeline(finalMatriz, v2);

                Vec4 normal4 = {f.normal.x, f.normal.y, f.normal.z, 0.0f};
                Vec4 normalView4 = multVetor(viewModelMatriz, normal4);
                Vec3 normalView = {normalView4.x, normalView4.y, normalView4.z};

                Vec4 centroView4 = multVetor(viewModelMatriz, v0);
                if (centroView4.h != 0) {
                    centroView4.x /= centroView4.h;
                    centroView4.y /= centroView4.h;
                    centroView4.z /= centroView4.h;
                }
                Vec3 centroView = {centroView4.x, centroView4.y, centroView4.z};

                unsigned char r, g, b;
                Vec3 posCamera = {0, 0, 0}; 
                
                calcularIluminacao(
                    normalView,
                    centroView,
                    luzCamera,      
                    obj.material,   
                    posCamera,
                    r, g, b
                );

                rasterizaTriangulo(p0, p1, p2, framebuffer, zBuffer, W, H, r, g, b);
            }
        }

        glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}