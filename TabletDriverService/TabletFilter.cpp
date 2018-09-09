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
    return CreateTimerQueueTimer(
        &timer,
        NULL, callback,
        NULL,
        0,
        timerInterval,
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
