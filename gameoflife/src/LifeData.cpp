#include "LifeData.h"

LifePattern::LifePattern()
{
	width = 0;
	height = 0;
	body = 0;
}

LifePattern::LifePattern(size_t width, size_t height, std::unique_ptr<char[]> body)
	: width(width)
	, height(height)
{
	this->body = std::move(body);
}

LifePattern::LifePattern(const LifePattern& other) {};

LifePattern::LifePattern(LifePattern&& other)
{
	width = std::exchange(other.width, 0);
	height = std::exchange(other.height, 0);
	body = std::move(other.body);
}

LifePattern& LifePattern::operator=(LifePattern&& other) noexcept
{
	width = std::exchange(other.width, 0);
	height = std::exchange(other.height, 0);
	body = std::move(other.body);
	return *this;
}


LifeRule::LifeRule()
{
	survivalRuleLength = 0;
	birthRuleLength = 0;
	survivalRule = 0;
	birthRule = 0;
}

LifeRule::LifeRule(size_t survivalRuleLength, size_t birthRuleLength, std::unique_ptr<int[]> survivalRule, std::unique_ptr<int[]> birthRule) 
	: survivalRuleLength(survivalRuleLength)
	, birthRuleLength(birthRuleLength)
	, survivalRule(std::move(survivalRule))
	, birthRule(std::move(birthRule))
{

}

LifeRule::LifeRule(const LifeRule& other) {};

LifeRule::LifeRule(LifeRule&& other) noexcept
	: survivalRuleLength(std::exchange(other.survivalRuleLength, 0))
	, birthRuleLength(std::exchange(other.birthRuleLength, 0))
	, survivalRule(std::move(other.survivalRule))
	, birthRule(std::move(other.birthRule))
{
}

LifeRule& LifeRule::operator=(LifeRule&& other) noexcept
{
	survivalRuleLength = std::exchange(other.survivalRuleLength, 0);
	birthRuleLength = std::exchange(other.birthRuleLength, 0);
	survivalRule = std::move(other.survivalRule);
	birthRule = std::move(other.birthRule);
	return *this;
}
