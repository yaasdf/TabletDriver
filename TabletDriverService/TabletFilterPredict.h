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

	PositionRingBuffer buffer;
	ChronoRingBuffer timeBuffer;
	Vector2D position;
	Vector2D lastTarget;
	int predictLength;
	PredictAlgorithm algorithm;

	double lastAngle;
	
	void SetPosition(Vector2D vector);
	bool GetPosition(Vector2D *outputVector);
	void SetAlgorithm(PredictAlgorithm a);
	void SetPredictLength(int p);
	void Update();
	void SetTargetTimer(Vector2D);
	void GetPositionPacket(Vector2D*);
	void UpdatePacket();
	void SetTarget(Vector2D targetVector);

	TabletFilterPredict();
	~TabletFilterPredict();

private:
	Vector2D UpdateRaw();
	Vector2D UpdateLinear();
	Vector2D UpdatePolygon();
	Vector2D UpdateCatmull();


};
