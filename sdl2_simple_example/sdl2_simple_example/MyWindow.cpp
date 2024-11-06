#include <exception>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_opengl.h>
#include "MyWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


using namespace std;

MyWindow::MyWindow(const char* title, unsigned short width, unsigned short height){
	open(title, width, height);
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForOpenGL(_window, _ctx);
    ImGui_ImplOpenGL3_Init("#version 130");

}

MyWindow::~MyWindow() {

    ImGui_ImplSDL2_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

    close();
}

void MyWindow::open(const char* title, unsigned short width, unsigned short height) {
    if (isOpen()) return;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    _window = SDL_CreateWindow( title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    if (!_window) throw exception(SDL_GetError());

    _ctx = SDL_GL_CreateContext(_window);
    if (!_ctx) throw exception(SDL_GetError());
    if (SDL_GL_MakeCurrent(_window, _ctx) != 0) throw exception(SDL_GetError());
    if (SDL_GL_SetSwapInterval(1) != 0) throw exception(SDL_GetError());
}

void MyWindow::close() {
    if (!isOpen()) return;

    SDL_GL_DeleteContext(_ctx);
    _ctx = nullptr;

    SDL_DestroyWindow(static_cast<SDL_Window*>(_window));
	_window = nullptr;
}



void MyWindow::swapBuffers() const {
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Menu")) {
            if (ImGui::MenuItem("Exit")) {
                // ... codi activació
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



    SDL_GL_SwapWindow(static_cast<SDL_Window*>(_window));
}

bool loadTexture(const char* filename);

bool MyWindow::processEvents(IEventProcessor* event_processor) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (event_processor) event_processor->processEvent(e);

        switch (e.type) {
        case SDL_QUIT:
            close();
            return false;

        case SDL_DROPFILE:
        {
            // Obtenemos el archivo soltado
            const char* dropped_file = e.drop.file;

            // Verificar si es un archivo FBX
            if (strstr(dropped_file, ".fbx") != nullptr) {
                // Aquí puedes cargar el archivo FBX usando ASSIMP
                printf("Archivo FBX soltado: %s\n", dropped_file);
                const aiScene* new_scene = aiImportFile(dropped_file, aiProcess_Triangulate | aiProcess_GenNormals);
                if (!new_scene) {
                    fprintf(stderr, "Error al cargar el archivo FBX: %s\n", aiGetErrorString());
                }
                else {
                    // Procesa el nuevo modelo
                    // Actualiza tus GameObjects y otras estructuras necesarias
                }
            }
            // Verificar si es un archivo de textura (PNG o DDS)
            else if (strstr(dropped_file, ".png") != nullptr || strstr(dropped_file, ".dds") != nullptr) {
                // Cargar la textura usando tu función loadTexture
                printf("Textura soltada: %s\n", dropped_file);
                if (!loadTexture(dropped_file)) {
                    fprintf(stderr, "Error al cargar la textura: %s\n", dropped_file);
                }
            }
            //SDL_free(dropped_file);  //liberar la memoria después de procesar el archivo
        }
        break;

        default:
            ImGui_ImplSDL2_ProcessEvent(&e);
            break;
        }
    }
    return true;
}

