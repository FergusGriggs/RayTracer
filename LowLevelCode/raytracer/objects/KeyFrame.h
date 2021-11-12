#pragma once

template<class T>
class KeyFrame
{
public:
	KeyFrame(const T& value, float time) :
		m_value(value),
		m_time(time)
	{
	}

	KeyFrame() :
		m_value(0.0f),
		m_time(0.0f)
	{
	}

	~KeyFrame()
	{
	}

	const T& getValue()
	{
		return m_value;
	}

	float getTime()
	{
		return m_time;
	}

private:
	T     m_value;
	float m_time;
};
