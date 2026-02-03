// André Ellen Rafael

#include "math.hpp"
#include "pipeline.hpp"
#include <iostream>
#include "cubo.hpp"

#include <GLFW/glfw3.h>

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

    const int W = 800;
    const int H = 600;

    unsigned char framebuffer[H][W][3];

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        framebuffer[300][400][0] = 255;
        framebuffer[300][400][1] = 0;
        framebuffer[300][400][2] = 0;

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