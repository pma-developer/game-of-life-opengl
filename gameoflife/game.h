#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <iostream>
#include "Shader.h"

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

    unsigned int gridDataTextures[2];
    unsigned int gridFBO;

    int gridWidth;
    int gridHeight;

    GLubyte* grid;

    bool flip = false;

	Game(int width, int height) : gridWidth(width), gridHeight(height)
	{
        grid = new GLubyte[gridWidth * gridHeight];

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

        initGridDataTexture();
        computationShader = Shader("./computation_shader.vert", "./computation_shader.frag");
        displayShader = Shader("./screen.vert", "./screen.frag");
        gridFBO = getGridFramebuffer(gridDataTextures[0]);
	}

    ~Game()
    {
        delete[] grid;
    }

    void initializeGrid(GLubyte defaultValue = 0)
    {
        for (int i = 0; i < gridWidth * gridHeight; ++i)
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
                if ((startX + x) < gridWidth && (startY + y) < gridHeight)
                {
                    grid[(startY + y) * gridWidth + (startX + x)] = pattern[y * patternWidth + x];
                }
            }
        }
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8_SNORM, gridWidth, gridHeight, 0, GL_RED, GL_UNSIGNED_BYTE, grid);

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


    void calculateNextGameState(unsigned int outputTexture, unsigned int inputTexture, int fullscreenVAO)
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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTexture);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, size[2], size[3]);
    }

    void displayGameGrid(float zoomLevel, glm::vec2 cameraCenter, unsigned int outputTexture, int fullscreenVAO)
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

    void iterateGame(float zoomLevel, glm::vec2 cameraCenter, int fullscreenVAO)
    {
        unsigned int outputTexture = gridDataTextures[flip ? 0 : 1];
        unsigned int inputTexture = gridDataTextures[flip ? 1 : 0];

        calculateNextGameState(outputTexture, inputTexture, fullscreenVAO);
        displayGameGrid(zoomLevel, cameraCenter, outputTexture, fullscreenVAO);

        flip = !flip;
    }

};

