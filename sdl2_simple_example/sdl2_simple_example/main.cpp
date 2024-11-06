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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



using namespace std;
using hrclock = chrono::high_resolution_clock;
using u8vec4 = glm::u8vec4;
using ivec2 = glm::ivec2;
using vec3 = glm::dvec3;

static const ivec2 WINDOW_SIZE(512, 512);
static const unsigned int FPS = 60;
static const auto FRAME_DT = 1.0s / FPS;

const aiScene* scene = nullptr;
GLuint textureID;

vector<vec3> vertices;
vector<glm::vec2> texCoords;  // Vector para almacenar las coordenadas de textura
vector<unsigned int> indices;

float x = 0.0f;
float y = 0.0f;
float z = 0.0f;

float cameraSpeed = 0.1f;  // Velocidad de movimiento de la cámara
float mouseSensitivity = 0.1f;  // Sensibilidad del ratón para la rotación

float angle = 0.0f;  // Variable de ángulo de rotación
float yaw = -90.0f;  // Ángulo de giro en Y (horizontal)
float pitch = 0.0f;  // Ángulo de giro en X (vertical)

float lastX = 400, lastY = 300;  // Última posición conocida del ratón
bool firstMouse = true;  // Para evitar saltos al inicio

// Función para cargar la textura desde el archivo "Baker_house.png"
bool loadTexture(const char* filename) {
    printf("Cargando textura: %s\n", filename); // Verifica la ruta
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
            nrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        return true;
    }
    else {
        fprintf(stderr, "Error al cargar la textura: %s\n", filename);
        return false;
    }
}

static void init_openGL() {
    glewInit();
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 1.0);

    // Cargar la textura "Baker_house.png"
    if (!loadTexture("Baker_house.png")) {
        printf("No se pudo cargar la textura.");
    }
    else
    {
        printf("Textura cargada.");
    }
}

// Función para dibujar el modelo cargado
static void drawModel() {
    glColor3f(1.0f, 1.0f, 1.0f);  // Color blanco para el modelo
    glBindTexture(GL_TEXTURE_2D, textureID);  // Enlazar la textura

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); i += 3) {
        for (int j = 0; j < 3; ++j) {
            int vertexIndex = indices[i + j];
            // Especifica la posición del vértice
            glVertex3f(vertices[vertexIndex].x, vertices[vertexIndex].y, vertices[vertexIndex].z);
            // Especifica las coordenadas de textura para el vértice actual
            glTexCoord2f(texCoords[vertexIndex].x, texCoords[vertexIndex].y);

            
        }
    }
    glEnd();
}

static void display_func() {

    //glEnable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configura la proyección ortográfica para la vista axonométrica
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z, 3.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glEnable(GL_TEXTURE_2D);  // Habilitar texturas
    glBindTexture(GL_TEXTURE_2D, textureID);  // Enlazar textura

    drawModel();  // Dibujar el modelo

    glDisable(GL_TEXTURE_2D);  // Desactivar texturas después de dibujar
}

//void keyboard(unsigned char key, int xMouse, int yMouse) {
//    const float speedMultiplier = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 2.0f : 1.0f;  // Duplicar velocidad si se presiona Shift
//
//    switch (key) {
//    case 'w': x += cameraSpeed * speedMultiplier * cos(glm::radians(yaw)); z -= cameraSpeed * speedMultiplier * sin(glm::radians(yaw)); break;
//    case 's': x -= cameraSpeed * speedMultiplier * cos(glm::radians(yaw)); z += cameraSpeed * speedMultiplier * sin(glm::radians(yaw)); break;
//    case 'a': x -= cameraSpeed * speedMultiplier * sin(glm::radians(yaw)); z -= cameraSpeed * speedMultiplier * cos(glm::radians(yaw)); break;
//    case 'd': x += cameraSpeed * speedMultiplier * sin(glm::radians(yaw)); z += cameraSpeed * speedMultiplier * cos(glm::radians(yaw)); break;
//    case 'q': y += cameraSpeed * speedMultiplier; break;  // Movimiento en el eje Y (arriba)
//    case ' ': y -= cameraSpeed * speedMultiplier; break;  // Movimiento en el eje Y (abajo)
//    }
//}
//void mouseMotion(int xMouse, int yMouse) {
//    if (firstMouse) {
//        lastX = xMouse;
//        lastY = yMouse;
//        firstMouse = false;
//    }
//
//    float xOffset = xMouse - lastX;
//    float yOffset = lastY - yMouse;  // El movimiento Y es invertido
//    lastX = xMouse;
//    lastY = yMouse;
//
//    xOffset *= mouseSensitivity;
//    yOffset *= mouseSensitivity;
//
//    yaw += xOffset;
//    pitch += yOffset;
//
//    // Limitar el pitch para evitar que la cámara gire 360 grados en el eje X
//    if (pitch > 89.0f) pitch = 89.0f;
//    if (pitch < -89.0f) pitch = -89.0f;
//}
//void mouseWheel(int wheelDelta) {
//    if (wheelDelta > 0) {  // Acercar
//        z -= cameraSpeed;
//    }
//    else {  // Alejar
//        z += cameraSpeed;
//    }
//}
//
//static void updateCamera() {
//    glm::vec3 front;
//    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    front.y = sin(glm::radians(pitch));
//    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    glm::vec3 cameraFront = glm::normalize(front);
//
//    // Crear la matriz de vista con la nueva orientación y posición
//    glm::vec3 cameraPosition = glm::vec3(x, y, z);
//    glm::vec3 cameraTarget = cameraPosition + cameraFront;
//    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
//
//    // Usar glm::lookAt para crear la matriz de vista
//    glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, up);
//    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_SIZE.x / (float)WINDOW_SIZE.y, 0.1f, 100.0f);
//
//    // Pasar las matrices de vista y proyección a OpenGL
//    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
//}


int main(int argc, char** argv) {
    MyWindow window("SDL2 Simple Example", WINDOW_SIZE.x, WINDOW_SIZE.y);
    init_openGL();

    const char* file = "BakerHouse.fbx";
    const struct aiScene* scene = aiImportFile(file, aiProcess_Triangulate | aiProcess_GenNormals);
    if (!scene) {
        fprintf(stderr, "Error al cargar el archivo: %s\n", aiGetErrorString());
        return -1;
    }

    //while (window.processEvents() && window.isOpen()) {
    //    const auto t0 = hrclock::now();

    //    updateCamera();  // Actualizar la cámara
    //    display_func();  // Dibujar el modelo

    //    window.swapBuffers();

    //    const auto t1 = hrclock::now();
    //    const auto dt = t1 - t0;
    //    if (dt < FRAME_DT) this_thread::sleep_for(FRAME_DT - dt);
    //}

    // Extraer vértices, índices y coordenadas de textura
    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];

        // Extraer los vértices y sus coordenadas de textura
        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            aiVector3D vertex = mesh->mVertices[v];
            vertices.push_back(vec3(vertex.x, vertex.y, vertex.z));

            // Verifica si el mesh tiene coordenadas de textura (UVs)
            if (mesh->mTextureCoords[0]) {
                // Asigna la primera capa de coordenadas de textura (si existen)
                aiVector3D texCoord = mesh->mTextureCoords[0][v];
                texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
            }
            else {
                // Si no tiene coordenadas de textura, agrega coordenadas (0,0)
                texCoords.push_back(ivec2(0.0f, 0.0f));
            }
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

