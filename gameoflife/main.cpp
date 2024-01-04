#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <iostream>
#include "Shader.h"
#include "main.h"
#include "game.h"

GLFWwindow* window;

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 1000;

float zoomFactor = 1.2;
float zoomLevel = 1.0f;


glm::vec2 cursorPosition;
glm::vec2 cursorDelta;

bool lmbPressed;


float vertices[] = {
    // positions //tex
    -1, 1, 0,   0, 1,// top left
    1, 1, 0,    1, 1,// top right
    -1, -1, 0,  0, 0,// bottom left
    1, -1, 0,   1, 0,// bottom right
};

unsigned int indices[] = {
    0, 1, 2,
    2, 1, 3
};

glm::vec2 cameraCenter = glm::vec2(0.5f, 0.5f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void processKeyboardInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}


glm::vec2 getLeftBottomAbsolutePos()
{
    return cameraCenter - glm::vec2((0.5 / zoomLevel));
}

glm::vec2 getRightTopAbsolutePos()
{
    return cameraCenter + glm::vec2((0.5 / zoomLevel));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        lmbPressed = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        lmbPressed = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    cursorPosition = glm::vec2(xpos, ypos);

    if (lmbPressed) 
    {
        glm::vec2 screenCursorPos = cursorDelta;

        screenCursorPos.x = -(screenCursorPos.x / (WINDOW_WIDTH * zoomLevel));
        screenCursorPos.y = screenCursorPos.y / (WINDOW_HEIGHT * zoomLevel);

        cameraCenter += screenCursorPos;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

    glm::vec2 screenCursorPos = cursorPosition;

    screenCursorPos.y = WINDOW_HEIGHT - screenCursorPos.y;

    screenCursorPos.x /= WINDOW_WIDTH;
    screenCursorPos.y /= WINDOW_HEIGHT;
    
    glm::vec2 absoluteCursorPosBeforeZoom = mix(getLeftBottomAbsolutePos(), getRightTopAbsolutePos(), screenCursorPos);

    zoomLevel = fmax(zoomLevel * (yoffset > 0 ? zoomFactor : 1 / zoomFactor), 1);

    glm::vec2 newAbsoluteCursorPos = mix(getLeftBottomAbsolutePos(), getRightTopAbsolutePos(), screenCursorPos);

    cameraCenter += (absoluteCursorPosBeforeZoom - newAbsoluteCursorPos);
}

int initWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Game of life", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    return 0;
}

int getFullscreenRectVAO()
{
    unsigned int screenVAO;
    glGenVertexArrays(1, &screenVAO);
    glBindVertexArray(screenVAO);

    unsigned int rectangleVBO;
    glGenBuffers(1, &rectangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int rectangleEBO;
    glGenBuffers(1, &rectangleEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangleEBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return screenVAO;
}
int main()
{

    int success = initWindow();
    if (success == -1)
    {
        return -1;
    }

    Game game = Game(2000, 2000);
    int fullscreenVAO = getFullscreenRectVAO();

    bool flip = false;
    while (!glfwWindowShouldClose(window))
    {
        processKeyboardInput(window);
        glClearColor(1, 0, 1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        game.iterateGame(zoomLevel, cameraCenter, fullscreenVAO);

        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }

        glm::vec2 oldCursorPos = cursorPosition;
        glfwPollEvents();
        glm::vec2 newCursorPos = cursorPosition;

        cursorDelta = newCursorPos - oldCursorPos;

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}