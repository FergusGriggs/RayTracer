#pragma once

#include <cmath>
#include <unordered_map>

#include "math_defines.h"

enum class EaseType
{
	eUnset,
	eLinear,
	eSine,
	eQuad,
	eCubic,
	eOvershoot,
	eElastic,
	eBounce
};

static EaseType getEaseTypeFromString(const std::string& str)
{
	EaseType retType = EaseType::eUnset;

	std::unordered_map<std::string, EaseType> types{
		{ "linear",    EaseType::eLinear },
		{ "sine",      EaseType::eSine },
		{ "quad",      EaseType::eQuad },
		{ "cubic",     EaseType::eCubic },
		{ "overshoot", EaseType::eOvershoot },
		{ "elastic",   EaseType::eElastic },
		{ "bounce",    EaseType::eBounce },
	};

	auto itr = types.find(str);
	if (itr != types.end())
	{
		retType = itr->second;
	}

	return retType;
};

static float getValueEasedIn(float x, EaseType easeType)
{
	switch (easeType)
	{
	case EaseType::eLinear:
		return x;
	case EaseType::eSine:
		return sin((x * M_PI) / 2.0f);

	case EaseType::eQuad:
		return 1.0f - (1.0f - x) * (1.0f - x);

	case EaseType::eCubic:
		return 1.0f - pow(1.0f - x, 3.0f);

	case EaseType::eOvershoot:
	{
		float c1 = 1.70158f;
		float c3 = c1 + 1.0f;
		return 1.0f + c3 * pow(x - 1.0f, 3.0f) + c1 * pow(x - 1.0f, 2.0f);
	}
	case EaseType::eElastic:
	{
		float c4 = (2.0f * M_PI) / 3.0f;
		return (x == 0.0f) ? 0.0f : (x == 1.0f) ? 1.0f : pow(2.0f, -10.0f * x) * sin((x * 10.0f - 0.75f) * c4) + 1.0f;
	}
	case EaseType::eBounce:
	{
		float n1 = 7.5625f;
		float d1 = 2.75f;

		if (x < 1.0f / d1)
		{
			return n1 * x * x;
		}
		else if (x < 2.0f / d1)
		{
			return n1 * (x -= 1.5f / d1) * x + 0.75f;
		}
		else if (x < 2.5f / d1)
		{
			return n1 * (x -= 2.25f / d1) * x + 0.9375f;
		}
		else
		{
			return n1 * (x -= 2.625f / d1) * x + 0.984375f;
		}
	}
	}

	return x;
}

static float getValueEasedOut(float x, EaseType easeType)
{
	switch (easeType)
	{
	case EaseType::eLinear:
		return x;
	case EaseType::eSine:
		return 1.0f - cos((x * M_PI) / 2.0f);

	case EaseType::eQuad:
		return x * x;

	case EaseType::eCubic:
		return x * x * x;

	case EaseType::eOvershoot:
	{
		float c1 = 1.70158f;
		float c3 = c1 + 1.0f;
		return c3 * x * x * x - c1 * x * x;
	}
	case EaseType::eElastic:
	{
		float c4 = (2.0f * M_PI) / 3.0f;
		return (x == 0.0f) ? 0.0f : (x == 1.0f) ? 1.0f : -pow(2.0f, 10.0f * x - 10.0f) * sin((x * 10.0f - 10.75f) * c4);
	}
	case EaseType::eBounce:
		return 1.0f - getValueEasedIn(1.0f - x, EaseType::eBounce);
	}

	return x;
}