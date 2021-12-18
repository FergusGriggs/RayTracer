#pragma once

#include "../MathDefines.h"
#include "../easings.h"

template<class T>
class KeyFrame
{
public:
	KeyFrame(const T& value, float time, EaseType easeIn, EaseType easeOut) :
		m_value(value),
		m_time(time),
		m_easeIn(easeIn),
		m_easeOut(easeOut)
	{
	}

	KeyFrame(const T& value, float time) :
		m_value(value),
		m_time(time),
		m_easeIn(EaseType::eUnset),
		m_easeOut(EaseType::eUnset)
	{
	}

	KeyFrame() :
		m_value(0.0f),
		m_time(0.0f),
		m_easeIn(EaseType::eUnset),
		m_easeOut(EaseType::eUnset)
	{
	}

	~KeyFrame()
	{
	}

	const T& getValue() const
	{
		return m_value;
	}

	float getTime() const
	{
		return m_time;
	}

	EaseType getEaseIn() const
	{
		return m_easeIn;
	}

	EaseType getEaseOut() const
	{
		return m_easeOut;
	}

private:
	T     m_value;
	float m_time;

	EaseType m_easeIn;
	EaseType m_easeOut;
};
