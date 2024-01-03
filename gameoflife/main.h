#pragma once

void CalculateNextGameState(unsigned int outputTexture, int fullscreenVAO, Shader& computationShader, unsigned int inputTexture);

void displayGameGrid(int fullscreenVAO, Shader& displayShader, unsigned int outputTexture);
