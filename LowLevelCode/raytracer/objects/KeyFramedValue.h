#pragma once

#include <vector>

#include "KeyFrame.h"

#include "../Vec3.h"
#include "../easings.h"

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

	void processKeyFrames(const T& defaultValue)
	{
		if (m_keyframes.empty())
		{
			m_keyframes.push_back(KeyFrame<T>(defaultValue, 0.0f));
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
			float frameATime = m_keyframes[keyFrameIndex].getTime();
			float frameBTime = m_keyframes[keyFrameIndex + 1].getTime();

			if (time >= frameATime && time < frameBTime)
			{
				float delta = (time - frameATime) / (frameBTime - frameATime);

				EaseType frameAEaseOut = m_keyframes[keyFrameIndex].getEaseOut();
				EaseType frameBEaseIn = m_keyframes[keyFrameIndex + 1].getEaseIn();

				if (frameAEaseOut != EaseType::eUnset && frameBEaseIn == EaseType::eUnset)
				{
					delta = getValueEasedOut(delta, frameAEaseOut);
				}
				else if (frameBEaseIn != EaseType::eUnset && frameAEaseOut == EaseType::eUnset)
				{
					delta = getValueEasedIn(delta, frameBEaseIn);
				}
				else if (frameAEaseOut != EaseType::eUnset && frameBEaseIn != EaseType::eUnset)
				{
					if (delta < 0.5f)
					{
						delta = 0.5f * getValueEasedOut(delta * 2.0f, frameAEaseOut);
					}
					else
					{
						delta = 0.5f + 0.5f * getValueEasedIn((delta - 0.5f) * 2.0f, frameBEaseIn);
					}
				}

				return keyFrameLerp(m_keyframes[keyFrameIndex].getValue(), m_keyframes[keyFrameIndex + 1].getValue(), delta);
			}
		}

		if (time >= m_keyframes[numKeyFrames - 1].getTime())
		{
			return m_keyframes[numKeyFrames - 1].getValue();
		}

		return T(0.0f);
	}

private:
	T                        m_currentValue;
	std::vector<KeyFrame<T>> m_keyframes;
};