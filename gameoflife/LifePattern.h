#include <memory>

#pragma once
struct LifePattern
{
	size_t width;
	size_t height;
	std::unique_ptr<char[]> body;

	LifePattern(size_t width, size_t height, std::unique_ptr<char[]> body);
	LifePattern(const LifePattern& other);

	LifePattern(LifePattern&& other);

	LifePattern& operator=(LifePattern&& other);

};

