#pragma once

#include <memory>

struct alignas(32) Event {
	int type;
};

class EventSystem {
private:
	Event* events = nullptr;
	int curSize = 0;
public:
	EventSystem() {
		events = (Event*)malloc(32 * 100);
	}

	void addEvent(void* event) {
		*(events + curSize++) = *(Event*)event;
	}

	void flushEvents() {
		for (int i = 0; i < curSize; i++) {

		}

		curSize = 0;
	}
};