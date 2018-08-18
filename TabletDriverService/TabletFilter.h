#pragma once
class TabletFilter {
public:
	virtual void SetTarget(Vector2D vector) = 0;
	virtual void SetPosition(Vector2D vector) = 0;
	virtual bool GetPosition(Vector2D *vector) = 0;
	virtual void Update() = 0;

	HANDLE timer;
	WAITORTIMERCALLBACK callback;
	double timerInterval;

	bool isEnabled;
	bool isValid;

	TabletFilter();

	bool StartTimer();
	bool StopTimer();

public:
	virtual void SetTargetPacket(Vector2D vector);
	virtual void SetTargetTimer(Vector2D vector);
	virtual void UpdatePacket();
	virtual void UpdateTimer();
	virtual void GetPositionPacket(Vector2D *vector);
	virtual void GetPositionTimer(Vector2D *vector);
};

