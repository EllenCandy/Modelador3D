// André Ellen Rafael

// #include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "pipeline.hpp"
#include "math.hpp"
#include <iostream>
#include "cubo.hpp"
#include "scene.hpp"

/*
Inicializa

cria janela
define largura e altura
inicializa framebuffer
inicializa zbuffer
*/

/*
Cena:

cria cubos
define posição, rotação, escala
define material de cada cubo
*/

/*
Câmera:

VRP
P
Y_up
Near / Far
window / viewport
*/

int main() {

    if (!glfwInit()) {
        std::cout << "Erro ao iniciar GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "CG - Modelador 3D", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    const int W = 800;
    const int H = 600;

    unsigned char framebuffer[H][W][3];

    Cena cena = criaCena();

    montaPipeline cam;

    Matriz4x4 finalMatriz = pipeline(cam);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

    // limpa framebuffer
    for(int y=0;y<H;y++)
        for(int x=0;x<W;x++) {
            framebuffer[y][x][0] = 0;
            framebuffer[y][x][1] = 0;
            framebuffer[y][x][2] = 0;
        }
 
    // desenha cada cubo como 1 pixel
    for (auto& obj : cena.objetos) {
 
        Vec4 centro = {0,0,0,1};
 
        Vec4 mundo = multVetor(obj.model, centro);
        Vec4 tela  = multVetor(finalMatriz, mundo);
 
        if (tela.h != 0) {
            tela.x /= tela.h;
            tela.y /= tela.h;
        }
 
        int x = (int)(tela.x);
        int y = (int)(tela.y);
 
        if (x >= 0 && x < W && y >= 0 && y < H) {
            framebuffer[y][x][0] = (unsigned char)(obj.material.r * 255);
            framebuffer[y][x][1] = (unsigned char)(obj.material.g * 255);
            framebuffer[y][x][2] = (unsigned char)(obj.material.b * 255);
        }
    }

        glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();

/*
    inicializaJanela(largura, altura);
    inicializaBuffers(largura, altura);

    std::vector<Cubo> cena = criaCena();

    Camera cam = configuraCamera(VRP, P, Y_up, Near, Far, window, viewport);

    bool rodando = true;

    while (rodando) {
        limpaBuffers();

        for (cada cubo da cena) {
           M_model = calculaModel(cubo);

           for (cada face do cubo) {
               transformaVertices();
               rasterizacao();
           }
      }

      desenhaFramebuffer();
    }
*/

return 0;

}