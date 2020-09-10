#pragma once

#include "Direct3D.h"

class CBoundingBox
{
private:
	Vector3 m_min;
	Vector3 m_max;

public:
	CBoundingBox() { m_min = Vector3::Zero; m_max = Vector3::Zero; }
	CBoundingBox(Vector3 min, Vector3 max) { m_min = min; m_max = max; }

	Vector3 GetMin() const { return m_min; }
	Vector3 GetMax() const { return m_max; }

	void SetMin(Vector3 min) { m_min = min; }
	void SetMax(Vector3 max) { m_max = max; }
};


