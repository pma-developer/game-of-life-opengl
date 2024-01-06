#include "LifePattern.h"

LifePattern::LifePattern(unsigned int width, unsigned int height, std::unique_ptr<char[]> body) :width(width), height(height)
{
	this->body = std::move(body);
}

LifePattern::LifePattern(LifePattern&& other)
{
	width = std::exchange(other.width, 0);
	height = std::exchange(other.height, 0);
	body = std::move(other.body);
}

LifePattern& LifePattern::operator=(LifePattern&& other) {
	body = std::move(other.body);
	return *this;
}
