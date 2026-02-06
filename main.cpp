// André Ellen Rafael

// #include <glad/glad.h>

#include <iostream>

#include <GLFW/glfw3.h>

#include "math.hpp"
#include "pipeline.hpp"
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
    CameraConfig cam;

    cam.VRP = {30, 40, 100}; // posição da câmera
    cam.P = {1, 2, 1};       // ponto para onde a câmera está olhando
    cam.Y_up = {0, 1, 0};    // vetor que define a orientação "para cima" da câmera

    // janela de visualização (window)
    cam.xmin = -10; cam.xmax = 10;
    cam.ymin = -8;  cam.ymax = 8;

    // viewport
    cam.umin = 0; cam.umax = W;
    cam.vmin = 0; cam.vmax = H;

    // planos de recorte
    cam.Near = 20; cam.Far = 160;

    // calcula matriz do pipeline
    // (M_view * M_proj * M_viewport)
    Matriz4x4 finalMatriz = calculaMatrizPipeline(cam);

    // triângulo de teste
    // 3 pontos no espaço do objeto (mundo)
    Vec4 tri[3] = {
        {-2, -1, 4, 1},
        { 3, -2, 5, 1},
        { 1,  3, 2, 1}
    };


    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

    // limpa framebuffer
    for(int y=0;y<H;y++)
    for(int x=0;x<W;x++)
        framebuffer[y][x][0] =
        framebuffer[y][x][1] =
        framebuffer[y][x][2] = 0;

    // passa cada ponto pelo pipeline
    Vec4 p0 = aplicaPipeline(finalMatriz, tri[0]);
    Vec4 p1 = aplicaPipeline(finalMatriz, tri[1]);
    Vec4 p2 = aplicaPipeline(finalMatriz, tri[2]);

    auto plot = [&](int x,int y){
        if(x>=0&&x<W&&y>=0&&y<H){
            framebuffer[y][x][0]=255;
            framebuffer[y][x][1]=255;
            framebuffer[y][x][2]=255;
        }
    };

    // desenha os pixels correspondentes
    plot((int)p0.x, (int)p0.y);
    plot((int)p1.x, (int)p1.y);
    plot((int)p2.x, (int)p2.y);

    glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate();

return 0;

}

// como vamos fazer a interação do usuário?

/*
teclado
mouse
menus

pensando aki:
posição da camera = setas
luz = mouse
material = menu
transformações = menu

*/