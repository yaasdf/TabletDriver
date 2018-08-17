#pragma once

#include "PositionRingBuffer.h"
#include "ChronoRingBuffer.h"
#include "Vector2D.h"

enum PredictAlgorithm
{
	LINEAR = 0,
	ALGORITHM_TOTAL
};

class TabletFilterPredict: public TabletFilter {
public:

	PositionRingBuffer buffer;
	ChronoRingBuffer timeBuffer;
	Vector2D position;
	Vector2D lastTarget;
	int predictLength;
	PredictAlgorithm algorithm;
	
	void SetPosition(Vector2D vector);
	bool GetPosition(Vector2D *outputVector);
	void SetTarget(Vector2D vector);
	void SetAlgorithm(PredictAlgorithm a);
	void SetPredictLength(int p);
	void Update();

	TabletFilterPredict();
	~TabletFilterPredict();

private:
	void UpdateRaw();
	void UpdateLinear();


};
