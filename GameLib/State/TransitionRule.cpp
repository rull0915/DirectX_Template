#include "pch.h"
#include "TransitionRule.h"

TransitionRule::TransitionRule(const std::function<bool()>& rule, const std::string& target)
	: m_rule{ rule }
	, m_targetName{ target }
{
}
