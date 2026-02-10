#include "scene.hpp"
 
Cena criaCena() {
    Cena c;
 
    // cubo base
    static Cubo cuboBase = criaCubo();
 
    // material vermelho
    Material mat1 = {0.5f, 0.7f, 0.5f, 20.0f, 1, 0, 0};
 
    // material azul
    Material mat2 = {0.5f, 0.7f, 0.5f, 10.0f, 0, 0, 1};
 
    // cubo 1
    Objeto o1;
    o1.mesh = &cuboBase;

    o1.model = multiplica(translacao(-30, -15, 5), escala(10.0f, 10.0f, 10.0f));
    o1.material = mat1;
 
    // cubo 2
    Objeto o2;
    o2.mesh = &cuboBase;

    o2.model = multiplica(translacao(30, 15, 0), escala(10.0f, 10.0f, 10.0f));
    o2.material = mat2;
 
    c.objetos.push_back(o1);
    c.objetos.push_back(o2);
 
    return c;
}