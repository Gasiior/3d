#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "spdlog/spdlog.h"
#include "XeEngine/ColorMaterial.h"

#define STB_IMAGE_IMPLEMENTATION 1
#include "3rdParty/stb/stb_image.h"

void SimpleShapeApplication::init() {

    // Tworzymy program cieniujący na podstawie plików z katalogu shaders
    auto program = xe::utils::create_program(
        { {GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
          {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"} });

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);  // chcemy widzieć całość, bez odrzucania ścian

    // Indeksy określają z których wierzchołków zbudowane są trójkąty:
    // dwa trójkąty na "ściany" i jeden na dach.
    unsigned int indices[] = {
    // podstawa (kwadrat z 2 trójkątów)
    0, 1, 2,
    0, 2, 3,

    // ściany boczne (4 trójkąty z wierzchołkiem 4)
    4, 0, 1,
    4, 1, 2,
    4, 2, 3,
    4, 3, 0
};
    // Każdy wierzchołek: pozycja (x,y,z) + kolor (r,g,b)
    float vertices[] = {
    // x,     y,     z,      r,    g,    b

    // podstawa – zielona
    -0.5f, -0.5f, -0.5f,   0.1f, 0.8f, 0.1f,   // 0: lewy tył
     0.5f, -0.5f, -0.5f,   0.1f, 0.8f, 0.1f,   // 1: prawy tył
     0.5f, -0.5f,  0.5f,   0.1f, 0.8f, 0.1f,   // 2: prawy przód
    -0.5f, -0.5f,  0.5f,   0.1f, 0.8f, 0.1f,   // 3: lewy przód

    // szczyt – czerwony
     0.0f,  0.5f,  0.0f,   0.9f, 0.1f, 0.1f    // 4: wierzchołek
};

#if __APPLE__
    auto u_modifiers_index = glGetUniformBlockIndex(program, "Modifiers");
    if (u_modifiers_index == -1) {
        std::cerr << "Cannot find Modifiers uniform block in program" << std::endl;
    } else {
        glUniformBlockBinding(program, u_modifiers_index, 0);
    }
#endif

    // UBO na macierz rzutowania * widoku (P * V)
    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // konfiguracja kamery
    set_camera(new Camera);

    camera()->look_at(
        glm::vec3(0, 1, 1),   // pozycja kamery
        glm::vec3(0, 0, 0),   // punkt na który patrzymy
        glm::vec3(0, 1, 0)    // wektor "góry"
    );

    int w, h;
    std::tie(w, h) = frame_buffer_size();
    camera()->perspective(glm::pi<float>() / 2.0f, (float)w / h, 0.1f, 100.f);

    // obsługa myszki dla kamery
    set_controler(new CameraControler(camera()));

    // Tworzymy VAO, VBO i EBO (index buffer)
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // wrzucamy wierzchołki do VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // wrzucamy indeksy do EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // atrybut 0: pozycja (3 floaty)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);

    // atrybut 1: kolor (3 floaty za pozycją)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // kolor tła + viewport
    glClearColor(0.81f, 0.81f, 0.80f, 1.0f);
    glViewport(0, 0, w, h);

    // wybieramy program i wiążemy blok UBO z macierzą PVM
    glUseProgram(program);
    GLuint idx = glGetUniformBlockIndex(program, "Transformations");
    glUniformBlockBinding(program, idx, 1);

    vao_ = VAO;
    program_ = program;
}

// wywoływane co klatkę – rysowanie sceny
void SimpleShapeApplication::frame() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // liczymy macierz P * V i aktualizujemy UBO
    auto PVM = camera()->projection() * camera()->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // używamy naszego VAO i programu
    glUseProgram(program_);
    glBindVertexArray(vao_);

    // najpierw rysujemy prostokąt (ściany) – 2 trójkąty
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

    // potem dach – ostatni trójkąt z indeksów
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0, 0, w, h);
    camera()->set_aspect((float)w / h);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }
}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}