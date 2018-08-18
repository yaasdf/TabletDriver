#include "stdafx.h"
#include "TabletFilterPredict.h"

#define LOG_MODULE "Predict"
#include "Logger.h"

//
// Constructor
//
TabletFilterPredict::TabletFilterPredict() {
	SetAlgorithm(LINEAR);
	timerInterval = 1;
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
void TabletFilterPredict::SetTargetTimer(Vector2D targetVector) {
	// do nothing
}
// Set position
void TabletFilterPredict::SetPosition(Vector2D vector) {
	position.x = vector.x;
	position.y = vector.y;
}
void TabletFilterPredict::GetPositionPacket(Vector2D* vector) {
	// do nothing
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
void TabletFilterPredict::UpdatePacket() {
	// Do nothing
}


void TabletFilterPredict::UpdateRaw() {
	position.x = lastTarget.x;
	position.y = lastTarget.y;
}

void TabletFilterPredict::UpdateLinear() {
	// let there be 3 sample points.
	// p1 is the point before latest, p2 is the latest point, and the predicting point.
	// These points have timestamps called t1, t2 and t3.
	// We are calculating p3's axis with the vertical between p1 and p2.

	timep t3 = high_resolution_clock::now();

	// One position in the buffer?
	if(buffer.count == 1) {
		position.Set(*buffer[0]);
		return;
	}
	Vector2D p1, p2;
	timep t1, t2;
	if (buffer.GetLatest(&p1, -1) && buffer.GetLatest(&p2, 0)) {
		timeBuffer.GetLatest(&t1, -1);
		timeBuffer.GetLatest(&t2, 0);
		int tSample = duration_cast<milliseconds>(t2 - t1).count();
		int tTotal = duration_cast<milliseconds>(t3 - t1).count() + predictLength;
		Vector2D p;
		p.x = p1.x + (p2.x - p1.x) / tSample * tTotal;
		p.y = p1.y + (p2.y - p1.y) / tSample * tTotal;
		
		// Skip prediction if dist is too high or low.
		//double distance2_3 = p.Distance(p1);
		double distance2_3 = (p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y);
		if (distance2_3 > 10 * 10 || distance2_3 < 0.05 * 0.05) {
			buffer.Reset();
			buffer.Add(p2);
			timeBuffer.Reset();
			timeBuffer.Add(t2);
			position.Set(p2);
			return;
		}

		position.Set(p);
	}
}