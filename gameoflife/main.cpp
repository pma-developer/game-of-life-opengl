#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <iostream>
#include "Shader.h"
#include "main.h"

GLFWwindow* window;

int WINDOW_WIDTH = 1000;
int WINDOW_HEIGHT = 1000;

const int GRID_WIDTH = 200;
const int GRID_HEIGHT = 200;

GLubyte glider[] = {
    0, 255, 0,
    0, 0, 255,
    255, 255, 255
};

GLubyte solidcolor[16*16] = {
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};
GLubyte grid[GRID_WIDTH * GRID_HEIGHT] = { 0 };

glm::vec2 cursorPosition;
glm::vec2 cursorDelta;

float zoomFactor = 1.2;
bool lmbPressed;

unsigned int gridDataTextures[2];
unsigned int gridFBO;

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

float zoomLevel = 1.0f; 
glm::vec2 cameraCenter = glm::vec2(0.5f, 0.5f);

void initializeGrid(GLubyte defaultValue = 0)
{
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; ++i)
    {
        grid[i] = defaultValue;
    }
}

void addPattern(const GLubyte* pattern, int patternWidth, int patternHeight,
    int startX, int startY)
{
    for (int y = 0; y < patternHeight; y++)
    {
        for (int x = 0; x < patternWidth; x++)
        {
            if ((startX + x) < GRID_WIDTH && (startY + y) < GRID_HEIGHT)
            {
                grid[(startY + y) * GRID_WIDTH + (startX + x)] = pattern[y * patternWidth + x];
            }
        }
    }
}

void setGridCoord(int x, int y, bool value)
{
    grid[y * GRID_WIDTH + x] = value;
}

bool getGridCoord(int x, int y)
{
    return grid[y * GRID_WIDTH + x];
}

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

    std::cout << "zoomLevel: " << zoomLevel << std::endl;
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

void setupTexture(unsigned int texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[4] = { 1, 1, 1, 1 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8_SNORM, GRID_WIDTH, GRID_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, grid);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void initGridDataTexture()
{
    glGenTextures(2, gridDataTextures);
    setupTexture(gridDataTextures[0]);
    setupTexture(gridDataTextures[1]);
}

unsigned int getGridFramebuffer(unsigned int texture)
{
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glFinish();
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return FBO;
}


void calculateNextGameState(unsigned int outputTexture, unsigned int inputTexture, int fullscreenVAO, Shader& computationShader)
{

    glBindFramebuffer(GL_FRAMEBUFFER, gridFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);
    glViewport(0, 0, GRID_WIDTH, GRID_HEIGHT);

    glBindVertexArray(fullscreenVAO);
    computationShader.use();
    computationShader.setInt("GRID_HEIGHT", GRID_HEIGHT);
    computationShader.setInt("GRID_WIDTH", GRID_WIDTH);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, inputTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void displayGameGrid(unsigned int outputTexture, int fullscreenVAO, Shader& displayShader)
{
    glBindVertexArray(fullscreenVAO);
    displayShader.use();
    displayShader.setFloat("zoomLevel", zoomLevel);
    displayShader.setVec2("cameraCenter", cameraCenter.x, cameraCenter.y);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, outputTexture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

int main()
{
    initializeGrid(0);
    addPattern(glider, 3, 3, 0, 0);
    addPattern(glider, 3, 3, 10, 0);
    addPattern(glider, 3, 3, 20, 0);
    addPattern(glider, 3, 3, 30, 0);
    addPattern(glider, 3, 3, 40, 0);
    addPattern(glider, 3, 3, 50, 0);
    addPattern(glider, 3, 3, 60, 0);
    addPattern(glider, 3, 3, 70, 0);
    addPattern(glider, 3, 3, 80, 0);
    addPattern(glider, 3, 3, 90, 0);
    addPattern(solidcolor, 16, 16, 100, 100);
    addPattern(solidcolor, 16, 16, 116, 100);
    int success = initWindow();
    if (success == -1)
    {
        return -1;
    }

    int fullscreenVAO = getFullscreenRectVAO();

    Shader computationShader = Shader("./computation_shader.vert", "./computation_shader.frag");
    Shader displayShader = Shader("./screen.vert", "./screen.frag");

    initGridDataTexture();
    gridFBO = getGridFramebuffer(gridDataTextures[0]);

    bool flip = false;
    while (!glfwWindowShouldClose(window))
    {
        processKeyboardInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        unsigned int outputTexture = gridDataTextures[flip ? 0 : 1];
        unsigned int inputTexture = gridDataTextures[flip ? 1 : 0];

        calculateNextGameState(outputTexture, inputTexture, fullscreenVAO, computationShader);
        displayGameGrid(outputTexture, fullscreenVAO, displayShader);
        
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }

        glm::vec2 oldCursorPos = cursorPosition;
        glfwPollEvents();
        glm::vec2 newCursorPos = cursorPosition;

        cursorDelta = newCursorPos - oldCursorPos;

        glfwSwapBuffers(window);
        flip = !flip;
    }

    glfwTerminate();
    return 0;
}