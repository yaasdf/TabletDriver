#pragma once

#include "PositionRingBuffer.h"
#include "ChronoRingBuffer.h"
#include "Vector2D.h"

enum PredictAlgorithm
{
	OFF = -1,
	LINEAR = 0,
	POLYGON,
	CATMULL,
	ALGORITHM_TOTAL
};

class TabletFilterPredict: public TabletFilter {
public:
	TabletFilterPredict();
	~TabletFilterPredict();

	PositionRingBuffer buffer;
	ChronoRingBuffer timeBuffer;
	Vector2D position;
	Vector2D lastTarget;
	int predictLength;
	PredictAlgorithm algorithm;

	void SetAlgorithm(PredictAlgorithm a);
	void SetPredictLength(int p);

	void SetPosition(Vector2D vector);
	bool GetPosition(Vector2D *outputVector);
	void SetTarget(Vector2D targetVector);

	void Update();

private:
	Vector2D UpdateRaw();
	Vector2D UpdateLinear();
	Vector2D UpdatePolygon();
	Vector2D UpdateCatmull();


};
