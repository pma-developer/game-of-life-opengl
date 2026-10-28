#include <memory>

#pragma once
struct LifePattern
{
	size_t width;
	size_t height;
	std::unique_ptr<char[]> body;

	LifePattern();

	LifePattern(size_t width, size_t height, std::unique_ptr<char[]> body);

	LifePattern(const LifePattern& other);

	LifePattern(LifePattern&& other);

	LifePattern& operator=(LifePattern&& other) noexcept;

};

struct LifeRule
{
	std::unique_ptr<int[]> survivalRule;
	std::unique_ptr<int[]> birthRule;

	size_t survivalRuleLength;
	size_t birthRuleLength;

	LifeRule();

	LifeRule(size_t survivalRuleLength, size_t birthRuleLength, std::unique_ptr<int[]> survivalRule, std::unique_ptr<int[]> birthRule);
	
	LifeRule(const LifeRule& other);

	LifeRule(LifeRule&& other) noexcept;

	LifeRule& operator=(LifeRule&& other) noexcept;

};

