#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader_m.h>
#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos   = glm::vec3(7.0f, 7.0f, 7.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;    // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("../src/shader/camera.vs", "../src/shader/camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    // world space positions of our cubes
    //Nivel Facil 01
    //Mapa 1
    std::vector<std::vector<char>> map = {
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X' }
    };
    //Mapa 02
   std::vector<std::vector<char>> map2 = {
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', 'X', ' ', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', 'X', ' ', 'X', 'X', ' ', 'X', 'X', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X' }
    };
    //Nivel Medio 02
    //Mapa 01
    std::vector<std::vector<char>> map3  = {
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', 'X', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', ' ', 'X', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
    };

    //Mapa 02
    std::vector<std::vector<char>> map4  = {
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', 'X', ' ',' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', 'X', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', ' ', 'X', ' ', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', 'X', 'X',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', ' ', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', 'X', 'X',' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', ' ', ' ', ' ',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', 'X', 'X','X', 'X', 'X', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
    };

    //Nivel Medio 02
    //Mapa 01
    std::vector<std::vector<char>> map5 = {
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', 'X', ' ',' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', ' ', 'X', ' ',' ', 'X', 'X', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', ' ', ' ', ' ',' ', 'X', 'X', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X', 'X', ' ','X', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X','X', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X','X', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', 'X','X', 'X' },
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
    };
    //Mapa 02
    std::vector<std::vector<char>> map6 = {
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', 'X',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', 'X',' ', ' ', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', 'X', ' ',' ', 'X', 'X', ' ',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', 'X', ' ', 'X', ' ', 'X',' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', ' ', ' ', ' ', ' ', ' ', 'X',' ', ' ', 'X', ' ',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', ' ', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', 'X', ' ', 'X',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', ' ', 'X', 'X', 'X', ' ', 'X', 'X', 'X',' ', 'X', ' ', 'X',' ', 'X', 'X', 'X',' ', 'X' },
        { 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',' ', 'X' },
        { 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X', 'X', 'X','X', 'X' },
    };
    std::vector<std::vector<std::vector<char>>> maps = {map, map2, map3, map4, map5, map6};

    // Tamaño de cada celda en el mapa
    float cellSize = 1.0f;

    // Recorrer el mapa y generar las posiciones de los cubos
    std::vector<glm::vec3> cubePositions;
    for (size_t i = 0; i < map.size(); i++) {
        for (size_t j = 0; j < map[i].size(); j++) {
            if (map[i][j] == 'X') {
                float x = static_cast<float>(j) * cellSize;
                float z = static_cast<float>(i) * cellSize;
                cubePositions.push_back(glm::vec3(x, 0.0f, -z)); // Agregar posición al arreglo
            }
        }
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("../src/texturas/muro.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);

    // render loop
    int currentMapIndex = 0;
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // ...

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        // create transformations
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the cubes
        glBindVertexArray(VAO);
        for (const auto& position : cubePositions)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            // Increment the index of the current map and wrap around if it exceeds the number of maps
            currentMapIndex = (currentMapIndex + 1) % maps.size();
            cubePositions.clear();

            // Retrieve the new cube positions for the current map
            const auto& currentMap = maps[currentMapIndex];
            for (size_t i = 0; i < currentMap.size(); i++)
            {
                for (size_t j = 0; j < currentMap[i].size(); j++)
                {
                    if (currentMap[i][j] == 'X')
                    {
                        float x = static_cast<float>(j) * cellSize;
                        float z = static_cast<float>(i) * cellSize;
                        cubePositions.push_back(glm::vec3(x, 0.0f, -z)); // Add position to the array
                    }
                }
            }
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
