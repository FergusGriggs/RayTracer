#pragma once

#include <vector>

#include "KeyFrame.h"
#include "../Vec3.h"

static bool keyFrameLerp(bool a, bool b, float delta)
{
	return a;
}

static float keyFrameLerp(float a, float b, float delta)
{
	return a + (b - a) * delta;
}

static Vec3f keyFrameLerp(const Vec3f& a, const Vec3f& b, float delta)
{
	return a + (b - a) * delta;
}

template<class T>
class KeyFramedValue
{
public:
	KeyFramedValue()
	{
	}

	~KeyFramedValue()
	{
	}

	void addKeyFrame(const KeyFrame<T>& keyFrame)
	{
		m_keyframes.push_back(keyFrame);
	}

	void processKeyFrames()
	{
		if (m_keyframes.empty())
		{
			m_keyframes.push_back(KeyFrame<T>());
		}
		else if (m_keyframes[0].getTime() != 0.0f) // Duplicate first
		{
			m_keyframes.insert(m_keyframes.begin(), m_keyframes[0]);
		}
	}

	void updateCurrentValue(float time)
	{
		m_currentValue = getValueAtTime(time);
	}

	const T& getCurrentValue() const
	{
		return m_currentValue;
	}

	T getValueAtTime(float time) const
	{
		if (m_keyframes.empty())
		{
			return T(0.0f);
		}
		else if (m_keyframes.size() == 1)
		{
			return m_keyframes[0].getValue();
		}

		int numKeyFrames = static_cast<int>(m_keyframes.size());
		for (int keyFrameIndex = 0; keyFrameIndex < numKeyFrames - 1; ++keyFrameIndex)
		{
			float frame1Time = m_keyframes[keyFrameIndex].getTime();
			float frame2Time = m_keyframes[keyFrameIndex + 1].getTime();

			if (time >= frame1Time && time < frame2Time)
			{
				float delta = (time - frame1Time) / (frame2Time - frame1Time);
				return keyFrameLerp(m_keyframes[keyFrameIndex].getValue(), m_keyframes[keyFrameIndex + 1].getValue(), delta);
			}
		}

		if (time >= m_keyframes[numKeyFrames - 1].getTime())
		{
			return m_keyframes[numKeyFrames - 1].getValue();
		}
	}

private:
	T                        m_currentValue;
	std::vector<KeyFrame<T>> m_keyframes;
};