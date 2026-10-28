#pragma once

void calculateNextGameState(unsigned int outputTexture, int fullscreenVAO, Shader& computationShader, unsigned int inputTexture);

void displayGameGrid(int fullscreenVAO, Shader& displayShader, unsigned int outputTexture);
