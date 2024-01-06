#include <memory>

#pragma once
struct LifePattern
{
	unsigned int width;
	unsigned int height;
	std::unique_ptr<char[]> body;

	LifePattern(unsigned int width, unsigned int height, std::unique_ptr<char[]> body);
	LifePattern(const LifePattern& other);

	LifePattern(LifePattern&& other);

	LifePattern& operator=(LifePattern&& other);

};

