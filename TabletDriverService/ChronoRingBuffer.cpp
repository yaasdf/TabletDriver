#include "stdafx.h"
#include "ChronoRingBuffer.h"


//
// Constructor
//
ChronoRingBuffer::ChronoRingBuffer() {
	maxLength = sizeof(buffer) / sizeof(timep);
	length = 0;
	count = 0;
	index = 0;
	isValid = false;
}


//
// Destructor
//
ChronoRingBuffer::~ChronoRingBuffer() {
}


//
// Set buffer length
//
void ChronoRingBuffer::SetLength(int len) {
	if(len > maxLength) {
		length = maxLength;
	} else {
		length = len;
	}
}


//
// Add position to buffer
//
void ChronoRingBuffer::Add(timep clock) {
	buffer[index] = clock;
	index++;
	count++;
	if(count > length) {
		count = length;
	}
	if(index >= length) {
		index = 0;
	}
	isValid = true;
}


//
// Get position history from the buffer
//
bool ChronoRingBuffer::GetLatest(timep *output, int delta) {
	int newIndex;

	// Buffer empty?
	if(count == 0) return false;

	// Valid delta?
	if(delta > 0 || delta <= -count) return false;

	newIndex = index - 1 + delta;

	// Limits
	if(newIndex < 0) newIndex = count + newIndex;

	if(newIndex < 0 || newIndex >= count) {
		return false;
	}

	*output = buffer[newIndex];
	return true;
}


//
// Reset buffer
//
void ChronoRingBuffer::Reset() {
	count = 0;
	index = 0;
	isValid = false;
}



//
// [] operator
//
timep *ChronoRingBuffer::operator[](std::size_t index) {
	return &(buffer[index]);
}