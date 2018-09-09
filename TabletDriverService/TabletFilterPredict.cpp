#include "stdafx.h"
#include "TabletFilterPredict.h"

#define LOG_MODULE "Predict"
#include "Logger.h"
#include <cmath>

const double PI = 3.141592653589793;


/////////////////////////////////////////////////////////////////////////////////

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


/////////////////////////////////////////////////////////////////////////////////

// Parameter settings

void TabletFilterPredict::SetAlgorithm(PredictAlgorithm a) {
	algorithm = a;
}

void TabletFilterPredict::SetPredictLength(int p) {
	predictLength = p;
}

/////////////////////////////////////////////////////////////////////////////////

// APIs

void TabletFilterPredict::SetTarget(Vector2D targetVector) {
	lastTarget.Set(targetVector);
	buffer.Add(targetVector);
	timeBuffer.Add(high_resolution_clock::now());
}

void TabletFilterPredict::SetPosition(Vector2D vector) {
	position.x = vector.x;
	position.y = vector.y;
}
bool TabletFilterPredict::GetPosition(Vector2D *outputVector) {
	outputVector->x = position.x;
	outputVector->y = position.y;
	return true;
}

/////////////////////////////////////////////////////////////////////////////////

// util functions

inline double getMs(const timep &t1, const timep &t2) {
	return duration_cast<nanoseconds>(t2 - t1).count() / 1000000.0;
}

inline int getMsD(const timep &t1, const timep &t2) {
	return duration_cast<milliseconds>(t2 - t1).count();
}

/////////////////////////////////////////////////////////////////////////////////

// position handling

void TabletFilterPredict::Update()
{
	// Deal with it if not enough samples
	switch (buffer.count)
	{
	case 0: position.Set(lastTarget); return;
	case 1: position.Set(*buffer[0]); return;
	}

	// Clear the buffers when a gap is detected
	timep t1, t2;
	timeBuffer.GetLatest(&t1, -1);
	timeBuffer.GetLatest(&t2, 0);
	if (getMs(t1, t2) > 20)
	{
		buffer.Reset();
		timeBuffer.Reset();
		buffer.Add(lastTarget);
		timeBuffer.Add(t2);
		position.Set(lastTarget);
		return;
	}

	Vector2D p;
	// leave it to complier to optimize this..
	switch (algorithm) {
	case LINEAR:
		p = UpdateLinear();
		break;
	case POLYGON:
		p = UpdatePolygon();
		break;
	case CATMULL:
		p = UpdateCatmull();
		break;
	default:
		p = UpdateRaw();
		break;
	}

	// Skip if distance to last point is too large.
	// I got around 20 when doing about several full area quick moves.
	double distance = lastTarget.Distance(p);
	if (distance < 12)
		position.Set(p);
	else
	{
		//if (distance < 50)
		//	LOG_ERROR("Distance too large: %lf\n", distance);
		position.Set(lastTarget);
	}
}

Vector2D TabletFilterPredict::UpdateRaw() {
	return lastTarget;
}

Vector2D MainLinear2(const Vector2D &p1, const Vector2D &p2,
	const timep &t1, const timep &t2, const timep &t3, int predictLength)
{
	// let there be 3 sample points.
	// p1 is the point before latest, p2 is the latest point, and the predicting point.
	// These points have timestamps called t1, t2 and t3.
	// We are calculating p3's axis with the vertical between p1 and p2.

	return p2 + (p2 - p1) / getMs(t1, t2) * (getMs(t2, t3) + predictLength);
}

Vector2D MainLinear3(const Vector2D &p1, const Vector2D &p2, const Vector2D &p3,
	const timep &t1, const timep &t2, const timep &t3, const timep &t4, int predictLength)
{
	// not implemented haha
	return MainLinear2(p2, p3, t2, t3, t4, predictLength);
}

Vector2D MainPolygon(const Vector2D &p1, const Vector2D &p2, const Vector2D &p3,
    const timep &t1, const timep &t2, const timep &t3, const timep &t4, int predictLength)
{
	// let there be 4 sample points: p1, p2, p3 and predicted p4, each with timestamps.
	// p1, p2 lines up vector vec1;
	// p2, p3 lines up vector vec2;
	// p3, p4 lines up vector vec3.
	// We are calculating the angle between vec1 and vec2, from which we get vec3.
	// The acceleration is ignored since it doesn't work as expected.

	// The Y axis is reversed
	Vector2D p0;
	Vector2D vec1, vec2;
	vec1.x = p2.x - p1.x;
	vec1.y = p1.y - p2.y;
	vec2.x = p3.x - p2.x;
	vec2.y = p2.y - p3.y;

	// FIXME double should not use "==0"
	if (vec1.x == 0 && vec1.y == 0 || vec2.x == 0 && vec2.y == 0) {
		return MainLinear3(p1, p2, p3, t1, t2, t3, t4, predictLength);
	}

	double s = atan2(vec2.y, vec2.x) - atan2(vec1.y, vec1.x);
	if (s > PI) s -= 2 * PI;
	if (s < -PI) s += 2 * PI;
	double a = abs(s);

	//double v12 = vec1.Distance(p0) / getMs(t1, t2);
	//double v23 = vec2.Distance(p0) / getMs(t2, t3);
	//double v34 = v23 - v12 + v23;
    double v34 = vec2.Distance(p0) / getMs(t2, t3); // v23

	Vector2D vec3;
	double cosa = cos(a);
	double sina = sin(a);
	if (s > 0) 	// counter-clockwise
	{
		vec3.x = vec2.x * cosa - vec2.y * sina;
		vec3.y = vec2.x * sina + vec2.y * cosa;
	}
	else	    // clockwise
	{
		vec3.x = vec2.x * cosa + vec2.y * sina;
		vec3.y = vec2.x * (-sina) + vec2.y * cosa;
	}
	double l2 = vec2.Distance(p0);
	double l3 = v34 * (getMs(t3, t4) + predictLength);
	double factor = l3 / l2;

	// reverse Y axis back here
	return Vector2D(
		p3.x + vec3.x * factor, 
		p3.y - vec3.y * factor
	);
}

Vector2D MainCatmull(const Vector2D &p1, const Vector2D &pp2, const Vector2D &pp3,
    const timep &t1, const timep &t2, const timep &t3, const timep &t4, int predictLength)
{
    Vector2D p2 = pp2;
    Vector2D p3 = pp3;
    Vector2D p4;
    // Polygonal reach to p4f and p5f, reuse p4f to get p5f
    timep t4f = t3 - t2 + t3;
    Vector2D p4f = MainPolygon(p1, p2, p3, t1, t2, t3, t4f, 0);
    timep t5f = t4f - t3 + t4f;
    Vector2D p5f = MainPolygon(p2, p3, p4f, t2, t3, t4f, t5f, 0);

    // f should be in [0, 1] here but some exceptions would happen
    double f = (getMs(t3, t4) + predictLength) / getMs(t3, t4f);

    // Decided to let it go since we are having fun
    //if (f < 1.5)
    {
        // for debug, to be commented
        //Vector2D p44 = MainPolygon(p1, p2, p3, t1, t2, t3, t4, predictLength);

        // reverse Y
        double y = tablet->settings.height;
        p2.y = y - p2.y;
        p3.y = y - p3.y;
        p4f.y = y - p4f.y;
        p5f.y = y - p5f.y;

        Vector2D p4;
        double f2 = f * f;
        double f3 = f2 * f;
        p4.x = 0.5 * (
            (p3.x * 2) +
            (-p2.x + p4f.x) * f +
            ((p2.x * 2) - (p3.x * 5) + (p4f.x * 4) - p5f.x) * f2 +
            (-p2.x + (p3.x * 3) - (p4f.x * 3) + p5f.x) * f3
            );
        p4.y = 0.5 * (
            (p3.y * 2) +
            (-p2.y + p4f.y) * f +
            ((p2.y * 2) - (p3.y * 5) + (p4f.y * 4) - p5f.y) * f2 +
            (-p2.y + (p3.y * 3) - (p4f.y * 3) + p5f.y) * f3
            );

        // reverse Y back here
        p4.y = y - p4.y;

        //LOG_INFO("%lf : %lf, %lf | %lf, %lf\n", f, p44.x, p44.y, p4.x, p4.y);
        return p4;
    }
    //else
    //{
    //    return MainPolygon(p1, p2, p3, t1, t2, t3, t4, predictLength);
    //}
}


/////////////////////////////////////////////////////////////////////////////////

// Update functions, called by Update()

Vector2D TabletFilterPredict::UpdateLinear()
{
	timep t3 = high_resolution_clock::now();

	Vector2D p1, p2, p3;
	timep t1, t2;
	if (buffer.GetLatest(&p1, -1) && buffer.GetLatest(&p2, 0)) {
		timeBuffer.GetLatest(&t1, -1);
		timeBuffer.GetLatest(&t2, 0);
		return MainLinear2(p1, p2, t1, t2, t3, predictLength);
	}

	// some error occurred
	LOG_ERROR("Invalid buffer\n");
	return position;
}

Vector2D TabletFilterPredict::UpdatePolygon()
{
	timep t4 = high_resolution_clock::now();

	// ... is this fallback necessary?
	if(buffer.count == 2) {
		return UpdateLinear();
	}

	Vector2D p1, p2, p3, p4;
	timep t1, t2, t3;
	if (buffer.GetLatest(&p1, -2) && buffer.GetLatest(&p2, -1) && buffer.GetLatest(&p3, 0)) {
		timeBuffer.GetLatest(&t1, -2);
		timeBuffer.GetLatest(&t2, -1);
		timeBuffer.GetLatest(&t3, 0);
		p4 = MainPolygon(p1, p2, p3, t1, t2, t3, t4, predictLength);
        return p4;
	}

	// some error occurred
	LOG_ERROR("Invalid buffer\n");
	return position;
}

Vector2D TabletFilterPredict::UpdateCatmull() {
	// let there be 4 sample points: p1, p2, p3 and predicted p4, each with timestamps.
	// And we needs to make TWO virtual point p4f and p5f, to make sure p4 is in the curve.
	// The curve is built with p2, p3, p4f and p5f, the p3~p4f part is valid.

	timep t4 = high_resolution_clock::now();

	// ... is this fallback necessary?
	if(buffer.count == 2) {
		return UpdateLinear();
	}

	Vector2D p1, p2, p3, p4;
	timep t1, t2, t3;
	if (buffer.GetLatest(&p1, -2) && buffer.GetLatest(&p2, -1) && buffer.GetLatest(&p3, 0)) {
		timeBuffer.GetLatest(&t1, -2);
		timeBuffer.GetLatest(&t2, -1);
		timeBuffer.GetLatest(&t3, 0);
        p4 = MainCatmull(p1, p2, p3, t1, t2, t3, t4, predictLength);
        return p4;
	}

	// some error occurred
	LOG_ERROR("Invalid buffer\n");
	return position;
}

