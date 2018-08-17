#pragma once

#include "Vector2D.h"
#include <chrono>

using namespace std::chrono;
using namespace std::chrono_literals;
typedef steady_clock::time_point timep;
class ChronoRingBuffer {
public:
	timep buffer[100];
	int maxLength;
	int length;
	int count;
	int index;
	bool isValid;

	void SetLength(int length);
	void Add(timep clock);
	bool GetLatest(timep *clock, int delta);
	void Reset();

	timep *operator[](std::size_t index);


	ChronoRingBuffer();
	~ChronoRingBuffer();
};

