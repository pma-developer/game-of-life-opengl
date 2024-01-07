#include <glad/glad.h>
#include <glm/glm/glm.hpp>
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

    Game(GLsizei width, GLsizei height, LifeRule lifeRule) : gridWidth(width), gridHeight(height), lifeRule(std::move(lifeRule))
	{
        grid = new GLubyte[gridWidth * gridHeight];

        initializeGrid(0);

	}

    ~Game()
    {
        delete[] grid;
    }

    void printAs2DArray(GLubyte* array, size_t width, size_t height) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                std::cout << array[i * width + j] << ' ';
            }
            std::cout << std::endl;
        }
    }
    void finishConfiguration()
    {
        vid_capture = VideoCapture("bad-apple60fps.mp4");
        if (!vid_capture.isOpened())
        {
            std::cout << "error on video file open " << std::endl;
        }

        Mat frame;
        bool isSuccess = vid_capture.read(frame);
        if (isSuccess) {
            uchar* arr = frame.isContinuous() ? frame.data : frame.clone().data;

            size_t length = frame.total() * frame.channels();
            size_t height = frame.rows;
            size_t width = frame.cols;
            videoFrameTexture = getInitedFrameTexture(width, height, arr);
        }
        else {
            std::cout << "error while reading video!" << std::endl;
        }

        initGridDataTexture();
        computationShader = Shader("./computation_shader.vert", "./computation_shader.frag", std::bind(&Game::injectCustomLifeRule, this, std::placeholders::_1));
        displayShader = Shader("./screen.vert", "./screen.frag");

        gridFBO = getGridFramebuffer(gridDataTextures[0]);
    }

    std::string injectCustomLifeRule(std::string shaderSource)
    {
        const std::string defineLifeRuleName = "OVERRIDE_LIFE_RULE";
        const std::string defineNeighborCountRuleName = "OVERRIDE_NEIGHBOR_COUNT_RULE";

        size_t lifeIfndefPos = shaderSource.find("#ifndef " + defineLifeRuleName);
        size_t neighborIfndefPos = shaderSource.find("#ifndef " + defineNeighborCountRuleName);

        std::string liveCountVarName = "liveCount";
        std::string resultLifeRuleMethod = "\n#define ";
        resultLifeRuleMethod.append(defineLifeRuleName);
        resultLifeRuleMethod.append("\n ");
        resultLifeRuleMethod.append("vec4 getNextState(int liveCount, vec4 selfState){");
        resultLifeRuleMethod.append("if(selfState.r == live.r){return ");
        for (int i = 0; i < lifeRule.survivalRuleLength; i++) {
            resultLifeRuleMethod.append(liveCountVarName + "==" + std::to_string(lifeRule.survivalRule[i]));
            if (i + 1 != lifeRule.survivalRuleLength) {
                resultLifeRuleMethod.append("||");
            }
        }
        resultLifeRuleMethod.append("?live:dead;}");

        resultLifeRuleMethod.append("else{return ");
        for (int i = 0; i < lifeRule.birthRuleLength; i++) {
            resultLifeRuleMethod.append(liveCountVarName + "==" + std::to_string(lifeRule.birthRule[i]));
            if (i + 1 != lifeRule.birthRuleLength) {
                resultLifeRuleMethod.append("||");
            }
        }
        resultLifeRuleMethod.append("?live:dead;}");
        resultLifeRuleMethod.append("}\n");

        shaderSource.insert(lifeIfndefPos, resultLifeRuleMethod);

        return shaderSource;
    }

    void initializeGrid(GLubyte defaultValue = 0)
    {
        for (size_t i = 0; i < gridWidth * gridHeight; ++i)
        {
            grid[i] = defaultValue;
        }
    }

    void addPattern(LifePattern lifePattern,
        size_t posX, size_t posY)
    {
        for (size_t y = 0; y < lifePattern.height; y++)
        {
            for (size_t x = 0; x < lifePattern.width; x++)
            {
                if ((posX + x) < gridWidth && (posY + y) < gridHeight)
                {
                    grid[(posY + y) * gridWidth + (posX + x)] = lifePattern.body[y * lifePattern.width + x];
                }
            }
        }
    }
    void addPattern(const GLubyte* pattern, size_t patternWidth, size_t patternHeight,
        size_t posX, size_t posY)
    {
        for (size_t y = 0; y < patternHeight; y++)
        {
            for (size_t x = 0; x < patternWidth; x++)
            {
                if ((posX + x) < gridWidth && (posY + y) < gridHeight)
                {
                    grid[(posY + y) * gridWidth + (posX + x)] = pattern[y * patternWidth + x];
                }
            }
        }
    }

    void setFrameTextureImage(unsigned int texture, size_t width, size_t height, GLubyte* data)
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    unsigned int getInitedFrameTexture(size_t width, size_t height, GLubyte* data)
    {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        float borderColor[4] = { 1, 1, 1, 1 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }

    void setupGridDataTexture(unsigned int texture)
    {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        float borderColor[4] = { 1, 1, 1, 1 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8_SNORM, gridWidth, gridHeight, 0, GL_RED, GL_UNSIGNED_BYTE, grid);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void initGridDataTexture()
    {
        glGenTextures(2, gridDataTextures);
        setupGridDataTexture(gridDataTextures[0]);
        setupGridDataTexture(gridDataTextures[1]);
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


    void calculateNextGameState(unsigned int outputTexture, unsigned int inputTexture, int fullscreenVAO, unsigned int frameTexture)
    {
        int size[4] = {};
        glGetIntegerv(GL_VIEWPORT, size);

        glBindFramebuffer(GL_FRAMEBUFFER, gridFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);
        glViewport(0, 0, gridWidth, gridHeight);

        glBindVertexArray(fullscreenVAO);
        computationShader.use();
        computationShader.setInt("GRID_HEIGHT", gridHeight);
        computationShader.setInt("GRID_WIDTH", gridWidth);
        computationShader.setInt("computationTexture", 0);
        computationShader.setInt("videoFrameTexture", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTexture);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, frameTexture);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, size[2], size[3]);
    }

    void displayGameGrid(float zoomLevel, glm::vec2 cameraCenter, unsigned int outputTexture, int fullscreenVAO)
    {
        displayShader.use();
        displayShader.setFloat("zoomLevel", zoomLevel);
        displayShader.setVec2("cameraCenter", cameraCenter.x, cameraCenter.y);
        displayShader.setInt("texture1", 0);
        displayShader.setInt("texture2", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, outputTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, videoFrameTexture);

        glBindVertexArray(fullscreenVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void iterateGame(float zoomLevel, glm::vec2 cameraCenter, int fullscreenVAO, bool isVideoPlaying)
    {
        if (isVideoPlaying == true) {
            Mat frame;
            bool isSuccess = vid_capture.read(frame);
            if (isSuccess) {
                Mat frameData = frame.clone();
                GLubyte* arr = frameData.data;
                size_t channels = frameData.channels();
                size_t length = frameData.total();
                size_t height = frameData.rows;
                size_t width = frameData.cols;
                setFrameTextureImage(videoFrameTexture, width, height, arr);
            }

            else {
                std::cout << "error while reading video!" << std::endl;
            }

            displayShader.setBool("bwColor", true);
        }
        unsigned int outputTexture = gridDataTextures[flip ? 0 : 1];
        unsigned int inputTexture = gridDataTextures[flip ? 1 : 0];

        calculateNextGameState(outputTexture, inputTexture, fullscreenVAO, videoFrameTexture);
        displayGameGrid(zoomLevel, cameraCenter, outputTexture, fullscreenVAO);

        flip = !flip;
    }

};

