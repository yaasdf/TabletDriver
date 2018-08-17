#include "stdafx.h"
#include "TabletFilterRaw.h"

#define LOG_MODULE "Raw"
#include "Logger.h"

//
// Constructor
//
TabletFilterRaw::TabletFilterRaw() {
	timerInterval = 1;
}


//
// Destructor
//
TabletFilterRaw::~TabletFilterRaw() {
}


//
// TabletFilter methods
//
void TabletFilterRaw::SetTarget(Vector2D targetVector) {
	targetVector.Set(position);
}
// Set position
void TabletFilterRaw::SetPosition(Vector2D vector) {
	position.Set(vector);
}
// Get position
bool TabletFilterRaw::GetPosition(Vector2D *outputVector) {
	outputVector->Set(position);
	return true;
}
void TabletFilterRaw::Update() {}