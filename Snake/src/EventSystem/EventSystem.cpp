#include "EventSystem.h"
#include <assert.h>

EventSystem* EventSystem::_instance = nullptr;

EventSystem::EventSystem()
{
	assert(_instance == nullptr && "Event system already initialized");
	events = (Event**)malloc(1000 * sizeof(uintptr_t));
	_instance = this;
}

void EventSystem::add(Event* event)
{
	*(events + curSize++) = event;
}

void EventSystem::flush()
{
	for (int i = 0; i < curSize; i++) {
		Event* event = events[i];

		if (delegates.find(event->getTypeId()) != delegates.end()) {
			for (auto& it : delegates[event->getTypeId()]) {
				it->invoke(event);
			}
		}
	}

	curSize = 0;
}