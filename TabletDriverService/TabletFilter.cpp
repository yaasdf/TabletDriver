#include "stdafx.h"
#include "TabletFilter.h"


TabletFilter::TabletFilter() {
	timer = NULL;
	timerInterval = 2;
	isValid = false;
	isEnabled = false;
}

//
// Start Timer
//
bool TabletFilter::StartTimer() {
	return CreateTimerQueueTimer(
		&timer,
		NULL, callback,
		NULL,
		0,
		(int)timerInterval,
		WT_EXECUTEDEFAULT
	);
}


//
// Stop Timer
//
bool TabletFilter::StopTimer() {
	if(timer == NULL) return false;
	bool result = DeleteTimerQueueTimer(NULL, timer, NULL);
	if(result) {
		timer = NULL;
	}
	return result;
}

void TabletFilter::SetTargetPacket(Vector2D vector) { SetTarget(vector); }
void TabletFilter::SetTargetTimer(Vector2D vector) { SetTarget(vector); }
void TabletFilter::UpdatePacket() { Update(); }
void TabletFilter::UpdateTimer() { Update(); }
void TabletFilter::GetPositionPacket(Vector2D* vector) { GetPosition(vector); }
void TabletFilter::GetPositionTimer(Vector2D* vector) { GetPosition(vector); }
