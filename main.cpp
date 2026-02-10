// André Ellen Rafael
// Modelador 3D - Trabalho Final de Computação Gráfica
// UNIOESTE - 2026

#include <iostream>
#include <limits>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "math.hpp"
#include "pipeline.hpp"
#include "cubo.hpp"
#include "scene.hpp"

#define largura 1100
#define altura 600
#define largura_comandos 300

int main() {

    if (!glfwInit()) {
        std::cout << "Erro ao iniciar GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(largura, altura, "CG - Modelador 3D", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    const int W = largura;
    const int H = altura;

    static unsigned char framebuffer[H][W][3];
    static float zBuffer[H][W];

    Cena cena = criaCena(); 
    
    CameraConfig cam;
    cam.VRP = {0, 0, 50};
    cam.P   = {0, 0, 0};
    cam.Y_up = {0, 1, 0};
    cam.Cu = cam.Cv = 0;
    cam.DP = 50;

    cam.Near = 10;
    cam.Far  = 500; 
    
    // limites da viewport
    cam.xmin = -10; cam.xmax = 10;
    cam.ymin = -8;  cam.ymax = 8;

    cam.umin = 0; cam.umax = (float)(W - largura_comandos);  // Área de desenho
    cam.vmin = 0; cam.vmax = (float)H;

    Light luzMundo;
    luzMundo.posicao = {30, 40, 100}; 
    luzMundo.Ia = 0.3f;
    luzMundo.Id = 0.7f;
    luzMundo.Is = 0.8f;

    int objetoSelecionado = 0;
    float translacao_x = 0.0f, translacao_y = 0.0f, translacao_z = 0.0f;
    float rotacao_x = 0.0f, rotacao_y = 0.0f, rotacao_z = 0.0f;
    float escala_uniforme = 1.0f;

    static Cubo cuboBase = criaCubo();
    float novoCubo_Ka = 0.2f, novoCubo_Kd = 0.7f, novoCubo_Ks = 0.5f;
    float novoCubo_m = 20.0f;
    float novoCubo_r = 1.0f, novoCubo_g = 1.0f, novoCubo_b = 1.0f;

    while (!glfwWindowShouldClose(window)) {
        
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(largura_comandos, altura), ImGuiCond_Always);
            
            ImGui::Begin("Controles", nullptr, 
                ImGuiWindowFlags_NoResize | 
                ImGuiWindowFlags_NoMove | 
                ImGuiWindowFlags_NoCollapse);

            if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Posicao (VRP):");
                ImGui::SliderFloat("VRP X", &cam.VRP.x, -100.0f, 100.0f);
                ImGui::SliderFloat("VRP Y", &cam.VRP.y, -100.0f, 100.0f);
                ImGui::SliderFloat("VRP Z", &cam.VRP.z, 10.0f, 500.0f);
                
                ImGui::Separator();
                
                ImGui::Text("Ponto de Interesse (P):");
                ImGui::SliderFloat("P X", &cam.P.x, -100.0f, 100.0f);
                ImGui::SliderFloat("P Y", &cam.P.y, -100.0f, 100.0f);
                ImGui::SliderFloat("P Z", &cam.P.z, -100.0f, 100.0f);
                
                ImGui::Separator();
                
                ImGui::Text("Planos de Recorte:");
                ImGui::SliderFloat("Near", &cam.Near, 0.001f, 50.0f);
                ImGui::SliderFloat("Far", &cam.Far, 50.0f, 500.0f);
                
                
                ImGui::Separator();
                
                ImGui::Text("Window:");
                ImGui::SliderFloat("xmin", &cam.xmin, -50.0f, 0.0f);
                ImGui::SliderFloat("xmax", &cam.xmax, 0.0f, 50.0f);
                ImGui::SliderFloat("ymin", &cam.ymin, -50.0f, 0.0f);
                ImGui::SliderFloat("ymax", &cam.ymax, 0.0f, 50.0f);

                ImGui::Separator();

                ImGui::SliderFloat("DP", &cam.DP, 0.0f, 300.0f);
            }

            if (ImGui::CollapsingHeader("Luz", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Text("Posicao:");
                ImGui::SliderFloat("Luz X", &luzMundo.posicao.x, -100.0f, 100.0f);
                ImGui::SliderFloat("Luz Y", &luzMundo.posicao.y, -100.0f, 100.0f);
                ImGui::SliderFloat("Luz Z", &luzMundo.posicao.z, -100.0f, 100.0f);
                
                ImGui::Separator();
                
                ImGui::Text("Intensidades:");
                ImGui::SliderFloat("Ambiente (Ia)", &luzMundo.Ia, 0.0f, 1.0f);
                ImGui::SliderFloat("Difusa (Id)", &luzMundo.Id, 0.0f, 1.0f);
                ImGui::SliderFloat("Especular (Is)", &luzMundo.Is, 0.0f, 1.0f);
            }

            if (ImGui::CollapsingHeader("Objetos", ImGuiTreeNodeFlags_DefaultOpen)) {
                
                ImGui::Text("Selecionar Objeto:");
                for (int i = 0; i < (int)cena.objetos.size(); i++) {
                    char nome_objeto[32];
                    sprintf(nome_objeto, "Cubo %d", i + 1);
                    if (ImGui::RadioButton(nome_objeto, objetoSelecionado == i)) {
                        objetoSelecionado = i;
                    }
                }
                
                ImGui::Separator();
                
                if (objetoSelecionado >= 0 && objetoSelecionado < (int)cena.objetos.size()) {
                    Material& mat = cena.objetos[objetoSelecionado].material;
                    
                    ImGui::Text("Material:");
                    ImGui::SliderFloat("Ka (Ambiente)", &mat.Ka, 0.0f, 1.0f);
                    ImGui::SliderFloat("Kd (Difusa)", &mat.Kd, 0.0f, 1.0f);
                    ImGui::SliderFloat("Ks (Especular)", &mat.Ks, 0.0f, 1.0f);
                    ImGui::SliderFloat("Brilho (m)", &mat.m, 1.0f, 100.0f);
                    
                    ImGui::Separator();
                    
                    ImGui::Text("Cor:");
                    ImGui::SliderFloat("R", &mat.r, 0.0f, 1.0f);
                    ImGui::SliderFloat("G", &mat.g, 0.0f, 1.0f);
                    ImGui::SliderFloat("B", &mat.b, 0.0f, 1.0f);
                }
                
                ImGui::Separator();
                
                ImGui::Text("Transformacoes:");
                
                ImGui::SliderFloat("Trans X", &translacao_x, -100.0f, 100.0f);
                ImGui::SliderFloat("Trans Y", &translacao_y, -100.0f, 100.0f);
                ImGui::SliderFloat("Trans Z", &translacao_z, -100.0f, 100.0f);
                
                ImGui::SliderFloat("Rot X (graus)", &rotacao_x, -180.f, 180.f);
                ImGui::SliderFloat("Rot Y (graus)", &rotacao_y, -180.f, 180.f);
                ImGui::SliderFloat("Rot Z (graus)", &rotacao_z, -180.f, 180.f);
                
                ImGui::SliderFloat("Escala", &escala_uniforme, 0.1f, 10.0f);
                
                if (ImGui::Button("Aplicar Transformacoes")) {
                    if (objetoSelecionado >= 0 && objetoSelecionado < (int)cena.objetos.size()) {
                        
                        // matriz temporária para guardar a concatenação das transformações
                        Matriz4x4 T = translacao(translacao_x, translacao_y, translacao_z);
                        
                        const float PI = 3.14159265359f;
                        float rx_rad = rotacao_x * PI / 180.0f;
                        float ry_rad = rotacao_y * PI / 180.0f;
                        float rz_rad = rotacao_z * PI / 180.0f;
                        
                        Matriz4x4 Rx = rotacaoX(rx_rad);
                        Matriz4x4 Ry = rotacaoY(ry_rad);
                        Matriz4x4 Rz = rotacaoZ(rz_rad);
                        Matriz4x4 S = escala(escala_uniforme, escala_uniforme, escala_uniforme);
                        
                        // Model = ModelAtual * T * Rz * Ry * Rx * S (transformação incremental)
                        Matriz4x4 temp = multiplica(T, multiplica(Rz, multiplica(Ry, multiplica(Rx, S))));
                        Matriz4x4 model = multiplica(cena.objetos[objetoSelecionado].model, temp);
                        
                        cena.objetos[objetoSelecionado].model = model;
                    }
                }
            }

            if (ImGui::CollapsingHeader("Criar Cubo")) {
                ImGui::Text("Parametros de Iluminacao:");
                
                ImGui::Separator();
                ImGui::Text("Reflexao Ambiente (Ka):");
                ImGui::SliderFloat("Ka", &novoCubo_Ka, 0.0f, 1.0f);
                
                ImGui::Separator();
                ImGui::Text("Reflexao Difusa (Kd):");
                ImGui::SliderFloat("Kd", &novoCubo_Kd, 0.0f, 1.0f);
                
                ImGui::Separator();
                ImGui::Text("Reflexao Especular (Ks):");
                ImGui::SliderFloat("Ks", &novoCubo_Ks, 0.0f, 1.0f);
                
                ImGui::Separator();
                ImGui::Text("Brilho (m):");
                ImGui::SliderFloat("m", &novoCubo_m, 1.0f, 100.0f);
                
                ImGui::Separator();
                ImGui::Text("Cor do Material:");
                ImGui::SliderFloat("R##novo", &novoCubo_r, 0.0f, 1.0f);
                ImGui::SliderFloat("G##novo", &novoCubo_g, 0.0f, 1.0f);
                ImGui::SliderFloat("B##novo", &novoCubo_b, 0.0f, 1.0f);
                
                ImGui::Separator();
                
                if (ImGui::Button("Criar Novo Cubo", ImVec2(largura_comandos - 20, 30))) {
                    Objeto novoCubo;
                    novoCubo.mesh = &cuboBase;
                    novoCubo.model = multiplica(translacao(0, 0, 0), escala(10.0f, 10.0f, 10.0f));
                    
                    Material novoMaterial;
                    novoMaterial.Ka = novoCubo_Ka;
                    novoMaterial.Kd = novoCubo_Kd;
                    novoMaterial.Ks = novoCubo_Ks;
                    novoMaterial.m = novoCubo_m;
                    novoMaterial.r = novoCubo_r;
                    novoMaterial.g = novoCubo_g;
                    novoMaterial.b = novoCubo_b;
                    
                    novoCubo.material = novoMaterial;
                    
                    cena.objetos.push_back(novoCubo);
                    objetoSelecionado = (int)cena.objetos.size() - 1;
                }
            }

            if (ImGui::CollapsingHeader("Info")) {
                ImGui::Text("FPS: %.1f", io.Framerate);
                ImGui::Text("Objetos: %d", (int)cena.objetos.size());
                ImGui::Text("Resolucao: %dx%d", largura, altura);
            }

            ImGui::End();
        }

        ImGui::Render();

        // Limpa a tela
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Limpa os buffers
        for(int y=0; y<H; y++) {
            for(int x=0; x<W; x++) {
                framebuffer[y][x][0] = 255; // fundo branco
                framebuffer[y][x][1] = 255; 
                framebuffer[y][x][2] = 255; 
                zBuffer[y][x] = std::numeric_limits<float>::infinity(); // zBuffer inicializa como infinito
            }
        }

        Matriz4x4 viewMatriz = multiplica(MatrizB(cam), MatrizA(cam));

        Vec4 luzPos4 = {luzMundo.posicao.x, luzMundo.posicao.y, luzMundo.posicao.z, 1.0f};
        Vec4 luzCameraPos4 = multVetor(viewMatriz, luzPos4);
        Light luzCamera;
        luzCamera.posicao = {luzCameraPos4.x, luzCameraPos4.y, luzCameraPos4.z};
        luzCamera.Ia = luzMundo.Ia;
        luzCamera.Id = luzMundo.Id;
        luzCamera.Is = luzMundo.Is;

        Matriz4x4 S_viewport = MatrizS(cam);

        std::vector<CuboTransformado> cubos_clip = pipeline(cam, cena);

        // Clipagem e exibição dos objetos em cena no próximo frame
        for (size_t idx = 0; idx < cubos_clip.size(); idx++) {
            const CuboTransformado& cubo = cubos_clip[idx];
            const Objeto& obj_original = cena.objetos[idx];
            
            // Mais pra garantir
            Matriz4x4 matEscala = escala(1.0f, 1.0f, 1.0f);
            Matriz4x4 modelFinal = multiplica(obj_original.model, matEscala);
            Matriz4x4 viewModel = multiplica(viewMatriz, modelFinal);

            // Itera por cada face de cada cubo  (cubo[idx])
            for (const Face& f : cubo.faces) {
                
                //  Como rasteriza por faces triangulares e nao quadradas, pega os 3 vertices de um triangulo 
                Vec4 v0_clip = cubo.vertices_clip[f.v1];
                Vec4 v1_clip = cubo.vertices_clip[f.v2];
                Vec4 v2_clip = cubo.vertices_clip[f.v3];

                std::vector<Vec4> poligono = {v0_clip, v1_clip, v2_clip};
                std::vector<Vec4> poligono_clipado = cliparPoligono(poligono);
                
                if (poligono_clipado.size() < 3) continue;
                
                Vec4 normal4 = {f.normal.x, f.normal.y, f.normal.z, 0.0f};
                Vec4 normalView4 = multVetor(viewModel, normal4);
                Vec3 normalView = {normalView4.x, normalView4.y, normalView4.z};
                
                Vec4 v0_original = obj_original.mesh->vertices[f.v1];

                Vec4 centroView4 = multVetor(viewModel, v0_original);
                if (centroView4.h != 0) {
                    centroView4.x /= centroView4.h;
                    centroView4.y /= centroView4.h;
                    centroView4.z /= centroView4.h;
                }
                Vec3 centroView = {centroView4.x, centroView4.y, centroView4.z};
                
                unsigned char r, g, b;
                calcularIluminacao(normalView, centroView, luzCamera, *(cubo.material), {0,0,0}, r, g, b);

                // itera pelos vértices do polígono
                for (size_t i = 1; i + 1 < poligono_clipado.size(); i++) {
                    Vec4 p0 = multVetor(S_viewport, poligono_clipado[0]);
                    Vec4 p1 = multVetor(S_viewport, poligono_clipado[i]);
                    Vec4 p2 = multVetor(S_viewport, poligono_clipado[i + 1]);
                    
                    if (p0.h != 0) { p0.x /= p0.h; p0.y /= p0.h; p0.z /= p0.h; }
                    if (p1.h != 0) { p1.x /= p1.h; p1.y /= p1.h; p1.z /= p1.h; }
                    if (p2.h != 0) { p2.x /= p2.h; p2.y /= p2.h; p2.z /= p2.h; }
                    
                    rasterizaTriangulo(p0, p1, p2, framebuffer, zBuffer, W - largura_comandos, H, r, g, b);
                }
            }
        }

        glDrawPixels(W, H, GL_RGB, GL_UNSIGNED_BYTE, framebuffer);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}
