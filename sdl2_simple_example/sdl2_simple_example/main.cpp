#include <GL/glew.h>
#include <chrono>
#include <thread>
#include <exception>
#include <glm/glm.hpp>
#include "MyWindow.h"
#include <stdio.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
using namespace std;

using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

const aiScene* scene = nullptr;  // Variable global
GLuint textureID;

vector<vec3> vertices;
vector<unsigned int> indices;

//Variable angulo rotación
float angle = 0.0f;

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];


static void init_openGL() {
	glewInit();
	if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.5, 0.5, 1.0);
    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++) {
            int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
            checkerImage[i][j][0] = (GLubyte)c;
            checkerImage[i][j][1] = (GLubyte)c;
            checkerImage[i][j][2] = (GLubyte)c;
            checkerImage[i][j][3] = (GLubyte)255;
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

// Función para dibujar el modelo cargado
static void drawModel() {
    glColor3f(1.0f, 1.0f, 1.0f); // Color blanco

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); i += 3) {
        // Primera coordenada de textura y vértice
        glTexCoord2f(0.0f, 0.0f);  // Coord textura (s, t) para el primer vértice
        glVertex3d(vertices[indices[i]].x, vertices[indices[i]].y, vertices[indices[i]].z);

        // Segunda coordenada de textura y vértice
        glTexCoord2f(1.0f, 0.0f);  // Coord textura (s, t) para el segundo vértice
        glVertex3d(vertices[indices[i + 1]].x, vertices[indices[i + 1]].y, vertices[indices[i + 1]].z);

        // Tercera coordenada de textura y vértice
        glTexCoord2f(0.5f, 1.0f);  // Coord textura (s, t) para el tercer vértice
        glVertex3d(vertices[indices[i + 2]].x, vertices[indices[i + 2]].y, vertices[indices[i + 2]].z);
    }
    glEnd();

}

//DIBUJAR TRIANGULO

//static void draw_triangle(const u8vec4& color, const vec3& center, double size) {
//	glColor4ub(color.r, color.g, color.b, color.a);
//	glBegin(GL_TRIANGLES);
//	glVertex3d(center.x, center.y + size, center.z);
//	glVertex3d(center.x - size, center.y - size, center.z);
//	glVertex3d(center.x + size, center.y - size, center.z);
//	glEnd();
//}

// Actualiza la función de visualización para mostrar el modelo en lugar del cubo
static void display_func() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configura la proyección ortográfica para la vista axonométrica
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-25.0, 25.0, -25.0, 25.0, -20.0, 20.0);  // Ajusta el área visible en la pantalla

    // Configura la matriz de modelo
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Mueve la cámara para alejar la vista axonométrica
    gluLookAt(8.0, 12.0, 10.0,   // Aleja la cámara moviéndola más lejos
        3.0, 0.0, 0.0,   // Punto de enfoque (el centro de la escena)
        0.0, 1.0, 0.0);  // Vector de arriba (eje Y)

    // Renderiza el modelo
    drawModel();
}


//DisplayFunct para dibujar CUBO

//static void display_func() {
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//    // Configura la matriz de proyección
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(45.0, (double)WINDOW_SIZE.x / (double)WINDOW_SIZE.y, 0.1, 100.0);
//
//    // Configura la matriz de modelo
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glTranslatef(0.0f, 0.0f, -5.0f); // Mueve la cámara hacia atrás
//
//	//draw_triangle(u8vec4(255, 0, 0, 255), vec3(0.0, 0.0, 0.0), 0.5);
//	
//	//// Dibuixar una línea 10 unitats amunt
//	//glLineWidth(2.0f);
//	//glBegin(GL_LINES);
//	//glVertex3f(0.f, 0.f, 0.f);
//	//glVertex3f(0.f, 10.f, 0.f);
//	//glEnd();
//
//    // Aplica la rotación
//    glRotatef(angle, 1.0f, 1.0f, 0.0f); // Rota alrededor del eje (1, 1, 0)
//
//   
//    //DIBUJA Y PINTA LAS CARAS
//
//    glBegin(GL_TRIANGLES);
//
//    // Color para la cara frontal
//    glColor3f(0.0f, 1.0f, 0.0f); // Color verde
//    // Cara frontal
//    glVertex3f(-1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, 1.0f, 1.0f);
//
//    glVertex3f(-1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, 1.0f, 1.0f);
//    glVertex3f(-1.0f, 1.0f, 1.0f);
//
//    // Color para la cara trasera
//    glColor3f(0.0f, 0.0f, 1.0f); // Color azul
//    // Cara trasera
//    glVertex3f(-1.0f, -1.0f, -1.0f);
//    glVertex3f(-1.0f, 1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, -1.0f);
//
//    glVertex3f(-1.0f, -1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, -1.0f);
//    glVertex3f(1.0f, -1.0f, -1.0f);
//
//    // Color para la cara izquerda
//    glColor3f(1.0f, 0.5f, 0.0f); // Color naranja
//    // Cara izquierda
//    glVertex3f(-1.0f, -1.0f, -1.0f);
//    glVertex3f(-1.0f, -1.0f, 1.0f);
//    glVertex3f(-1.0f, 1.0f, 1.0f);
//
//    glVertex3f(-1.0f, -1.0f, -1.0f);
//    glVertex3f(-1.0f, 1.0f, 1.0f);
//    glVertex3f(-1.0f, 1.0f, -1.0f);
//
//    // Color para la cara derecha
//    glColor3f(1.0f, 0.0f, 0.0f); // Color rojo
//    // Cara derecha
//    glVertex3f(1.0f, -1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, 1.0f);
//
//    glVertex3f(1.0f, -1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, 1.0f);
//    glVertex3f(1.0f, -1.0f, 1.0f);
//
//    // Color para la cara superior
//    glColor3f(1.0f, 1.0f, 1.0f); // Color blanco
//    // Cara superior
//    glVertex3f(-1.0f, 1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, 1.0f);
//
//    glVertex3f(-1.0f, 1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, 1.0f);
//    glVertex3f(-1.0f, 1.0f, 1.0f);
//
//    // Color para la cara inferior
//    glColor3f(1.0f, 1.0f, 0.0f); // Color amarillo
//    // Cara inferior
//    glVertex3f(-1.0f, -1.0f, -1.0f);
//    glVertex3f(1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, -1.0f, -1.0f);
//
//    glVertex3f(-1.0f, -1.0f, -1.0f);
//    glVertex3f(-1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, -1.0f, 1.0f);
//
//    glEnd();
//
//
//    // DIBUJAR LOS EDGES
//
//    glColor3f(0.0f, 0.0f, 0.0f); // Color negro para las aristas
//    glBegin(GL_LINES);
//
//    // Edges frontales
//    glVertex3f(-1.0f, -1.0f, 1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, -1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
//    glVertex3f(1.0f, 1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
//    glVertex3f(-1.0f, 1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
//
//    // Edges traseras
//    glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(1.0f, -1.0f, -1.0f);
//    glVertex3f(1.0f, -1.0f, -1.0f); glVertex3f(1.0f, 1.0f, -1.0f);
//    glVertex3f(1.0f, 1.0f, -1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);
//    glVertex3f(-1.0f, 1.0f, -1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
//
//    // Conecta los edges frontales y traseros
//    glVertex3f(-1.0f, -1.0f, -1.0f); glVertex3f(-1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, -1.0f, -1.0f); glVertex3f(1.0f, -1.0f, 1.0f);
//    glVertex3f(1.0f, 1.0f, -1.0f); glVertex3f(1.0f, 1.0f, 1.0f);
//    glVertex3f(-1.0f, 1.0f, -1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);
//
//    glEnd();
//
//    // Incrementa el ángulo de rotación
//    angle += 1.0f;
//}

//Main para el CUBO

//int main1(int argc, char** argv) {
//	MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
//
//	init_openGL();
//
//	while(window.processEvents() && window.isOpen()) {
//		const auto t0 = hrclock::now();
//		display_func();
//		window.swapBuffers();
//		const auto t1 = hrclock::now();
//		const auto dt = t1 - t0;
//		if(dt<FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
//	}
//
//	return 0;
//}

//Main LaserGun

int main(int argc, char** argv) {
    // Inicializa la ventana y OpenGL
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    init_openGL();

    // Cargar el modelo 3D
    const char* file = "LaserGun_P1.fbx"; // Ruta del archivo a cargar
    const struct aiScene* scene = aiImportFile(file, aiProcess_Triangulate | aiProcess_GenNormals);
    if (!scene) {
        fprintf(stderr, "Error en cargar el archivo: %s\n", aiGetErrorString());
        return -1;
    }

    // Extraer vértices e índices de la malla cargada
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // Extraer los vértices
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.emplace_back(vec3(vertex.x, vertex.y, vertex.z));
        }

        // Extraer los índices de las caras
        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    aiReleaseImport(scene);

    // Bucle principal de renderizado
    while (window.processEvents() && window.isOpen()) {
        const auto t0 = hrclock::now();
        display_func();
        window.swapBuffers();
        const auto t1 = hrclock::now();
        const auto dt = t1 - t0;
        if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    }

    return 0;
}