// André Ellen Rafael

#include "math.cpp"
#include "pipeline.cpp"
#include <iostream>

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

return 0;

}