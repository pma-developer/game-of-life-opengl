#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include "Shader.h"
#include "LifeData.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const static GLubyte glider[] = {
    0, 255, 0,
    0, 0, 255,
    255, 255, 255
};

const static GLubyte solidcolor[16 * 16] = {
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
#pragma once
class Game
{
public:
    Shader computationShader;
    Shader displayShader;

    VideoCapture vid_capture;

    unsigned int videoFrameTexture;
    unsigned int gridDataTextures[2];
    unsigned int gridFBO;

    GLsizei gridWidth;
    GLsizei gridHeight;

    GLubyte* grid;

    LifeRule lifeRule;

    bool flip = false;

    Game(GLsizei width, GLsizei height, LifeRule lifeRule);

    ~Game();

    void printAs2DArray(GLubyte* array, size_t width, size_t height);

    void finishConfiguration();

    std::string injectCustomLifeRule(std::string shaderSource);

    void initializeGrid(GLubyte defaultValue = 0);

    void addPattern(LifePattern lifePattern, size_t posX, size_t posY);

    void addPattern(const GLubyte* pattern, size_t patternWidth, size_t patternHeight, size_t posX, size_t posY);

    void setFrameTextureImage(unsigned int texture, size_t width, size_t height, GLubyte* data);

    unsigned int getInitedFrameTexture(size_t width, size_t height, GLubyte* data);

    void setupGridDataTexture(unsigned int texture);

    void initGridDataTexture();

    unsigned int getGridFramebuffer(unsigned int texture);

    void calculateNextGameState(unsigned int outputTexture, unsigned int inputTexture, int fullscreenVAO, unsigned int frameTexture);

    void displayGameGrid(float zoomLevel, glm::vec2 cameraCenter, unsigned int outputTexture, int fullscreenVAO);

    void iterateGame(float zoomLevel, glm::vec2 cameraCenter, int fullscreenVAO, bool isVideoPlaying);
};

