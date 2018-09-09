#include "stdafx.h"
#include "TabletFilter.h"
#define LOG_MODULE "Filter"
#include "Logger.h"


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
    if (timerInterval > 0)
    {
        LOG_INFO("Starting Timer with Interval %dms\n", timerInterval);
        return CreateTimerQueueTimer(
            &timer,
            NULL, callback,
            NULL,
            0,
            timerInterval,
            WT_EXECUTEDEFAULT
        );
    }
    else
        return false;
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
bool TabletFilter::GetPositionPacket(Vector2D* vector) { return GetPosition(vector); }
bool TabletFilter::GetPositionTimer(Vector2D* vector) { return GetPosition(vector); }
