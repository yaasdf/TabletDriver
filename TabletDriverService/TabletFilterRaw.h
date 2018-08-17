#pragma once

#include "PositionRingBuffer.h"
#include "Vector2D.h"

class TabletFilterRaw : public TabletFilter {
public:

	Vector2D position;

	void SetPosition(Vector2D vector);
	bool GetPosition(Vector2D *outputVector);
	void SetTarget(Vector2D vector);
	void Update();

	TabletFilterRaw();
	~TabletFilterRaw();
};

