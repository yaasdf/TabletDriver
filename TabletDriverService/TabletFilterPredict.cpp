#include "stdafx.h"
#include "TabletFilterPredict.h"

#define LOG_MODULE "Predict"
#include "Logger.h"

//
// Constructor
//
TabletFilterPredict::TabletFilterPredict() {
	SetAlgorithm(LINEAR);
	predictLength = 0;
	buffer.SetLength(10);
	timeBuffer.SetLength(10);
}


//
// Destructor
//
TabletFilterPredict::~TabletFilterPredict() {
}


//
// TabletFilter methods
//
void TabletFilterPredict::SetAlgorithm(PredictAlgorithm a) {
	algorithm = a;
}

void TabletFilterPredict::SetPredictLength(int p) {
	predictLength = p;
}

void TabletFilterPredict::SetTarget(Vector2D targetVector) {
	lastTarget.Set(targetVector);
	buffer.Add(targetVector);
	timeBuffer.Add(high_resolution_clock::now());
}
// Set position
void TabletFilterPredict::SetPosition(Vector2D vector) {
	position.x = vector.x;
	position.y = vector.y;
}
// Get position
bool TabletFilterPredict::GetPosition(Vector2D *outputVector) {
	outputVector->x = position.x;
	outputVector->y = position.y;
	return true;
}
void TabletFilterPredict::Update() {
	// leave it to complier to optimize this..
	switch (algorithm) {
	case LINEAR:
		UpdateLinear();
		break;
	default:
		UpdateRaw();
		break;
	}
}


void TabletFilterPredict::UpdateRaw() {
	position.x = lastTarget.x;
	position.y = lastTarget.y;
}

void TabletFilterPredict::UpdateLinear() {
	timep t3 = high_resolution_clock::now();

	// One position in the buffer?
	if(buffer.count == 1) {
		position.x = buffer[0]->x;
		position.y = buffer[0]->y;
		return;
	}
	Vector2D p1, p2;
	timep t1, t2;
	if (buffer.GetLatest(&p1, 0) && buffer.GetLatest(&p2, -1)) {
		timeBuffer.GetLatest(&t1, 0);
		timeBuffer.GetLatest(&t2, -1);
		int t = duration_cast<milliseconds>(t2 - t1).count();
		int tl = duration_cast<milliseconds>(t3 - t1).count() + predictLength;
		position.x = p1.x + (p2.x - p1.x) / t * tl;
		position.y = p1.y + (p2.y - p1.y) / t * tl;
	}
}