#pragma once

#include <cmath>
#include "Vectors.hpp"

namespace Utils
{
	template <typename T>
	inline T Sign(T val) {
		if (val < 0)
		{
			return val * -1;
		}

		return val;
	}

	inline Vector2 Sign(const Vector2& val)
	{
		return Vector2(Sign(val.x), Sign(val.y));
	}

	inline Vector3 Sign(const Vector3& val)
	{
		return Vector3(Sign(val.x), Sign(val.y), Sign(val.z));
	}

	inline float Length(const Vector2& val1, const Vector2& val2)
	{
		return sqrtf(powf(Sign(val1.x - val2.x), 2) + powf(Sign(val1.y - val2.y), 2));
	}
}
