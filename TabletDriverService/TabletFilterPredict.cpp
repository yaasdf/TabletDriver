#include "stdafx.h"
#include "TabletFilterPredict.h"

#define LOG_MODULE "Predict"
#include "Logger.h"
#include <cmath>

#ifndef PI
#define PI 3.141592653589793
#endif // !PI


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
	case POLYGON:
		UpdatePolygon();
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

inline double getMs(const timep &t1, const timep &t2) {
	return duration_cast<nanoseconds>(t2 - t1).count() / 1000000;
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
	Vector2D p1, p2, p3;
	timep t1, t2;
	if (buffer.GetLatest(&p1, -1) && buffer.GetLatest(&p2, 0)) {
		timeBuffer.GetLatest(&t1, -1);
		timeBuffer.GetLatest(&t2, 0);
		int tSample = (int)getMs(t1, t2);
		int tTotal = (int)getMs(t1, t3) + predictLength;
		p3.x = p1.x + (p2.x - p1.x) / tSample * tTotal;
		p3.y = p1.y + (p2.y - p1.y) / tSample * tTotal;
		
		// Skip prediction if dist is too high or low.
		double distance2_3 = (p3.x - p2.x) * (p3.x - p2.x) + (p3.y - p2.y) * (p3.y - p2.y);
		if (distance2_3 > 10 * 10 || distance2_3 < 0.05 * 0.05) {
			buffer.Reset();
			buffer.Add(p2);
			timeBuffer.Reset();
			timeBuffer.Add(t2);
		} else
			position.Set(p3);
	} else 
		position.Set(p2);
}

void TabletFilterPredict::UpdatePolygon() {
	// let there be 4 sample points: p1, p2, p3 and predicted p4, each with timestamps.
	// p1, p2 lines up vector vec1;
	// p2, p3 lines up vector vec2;
	// p3, p4 lines up vector vec3.
	// We are calculating the angle between vec1 and vec2, from which we get vec3.
	// The velocity between p1, p2 and p3 is also considerated.

	timep t4 = high_resolution_clock::now();

	// One position in the buffer?
	if(buffer.count == 1) {
		position.Set(*buffer[0]);
		return;
	}

	// ... is this fallback necessary?
	if(buffer.count == 2) {
		UpdateLinear();
		return;
	}

	Vector2D p0, p1, p2, p3, p4;
	timep t1, t2, t3;
	p0.x = 0;
	p0.y = 0;
	if (buffer.GetLatest(&p1, -2) && buffer.GetLatest(&p2, -1) && buffer.GetLatest(&p3, 0)) {
		timeBuffer.GetLatest(&t1, -2);
		timeBuffer.GetLatest(&t2, -1);
		timeBuffer.GetLatest(&t3, 0);

		Vector2D vec1, vec2;
		vec1.x = p2.x - p1.x;
		vec1.y = p2.y - p1.y;
		vec2.x = p3.x - p2.x;
		vec2.y = p3.y - p2.y;
        
        if (vec1.x == 0 && vec1.y == 0 || vec2.x == 0 && vec2.y == 0 || getMs(t3, t4) + predictLength == 0) {
            UpdateLinear();
            return;
        }

        double s = atan2(vec2.y, vec2.x) - atan2(vec1.y, vec1.x);
        if (s > PI) s -= 2 * PI;
        else if (s < -PI) s += 2 * PI;
        double a = PI - s;
        
		if (abs(a) < 0.001)	// same line
		{
            UpdateLinear();
            return;
		}
        else {
            double t12 = getMs(t1, t2);
            double t23 = getMs(t2, t3);
            double t34 = getMs(t3, t4) + predictLength;
            double v12 = vec1.Distance(p0) / t12;
            double v23 = vec2.Distance(p0) / t23;
            double dv = v23 - v12;
            double l = (v23 - v12 + v23) * t34;
            double factor = l / vec2.Distance(p0);
            Vector2D vec3;
            if (a > 0) 	// counter-clockwise
            {
                vec3.x = (vec2.x * cos(a) - vec2.y * sin(a)) * factor;
                vec3.y = (vec2.y * sin(a) + vec2.y * cos(a)) * factor;
            }
            else	    // clockwise
            {
                vec3.x = (vec2.x * cos(a) - vec2.y * sin(-a)) * factor;
                vec3.y = (vec2.y * sin(-a) + vec2.y * cos(a)) * factor;
            }
            p4.x = p3.x + vec3.x;
            p4.y = p3.y + vec3.y;
            position.Set(p4);
        }
	}
	else
		position.Set(p3);

}