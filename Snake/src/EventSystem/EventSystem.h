#pragma once

#include <memory>
#include "Events.h"
#include <vector>
#include <unordered_map>

//enum class SystemEventTypes {
//	LeftMouseClick, RightMouseClick, MiddleMouseClick,
//	LeftMouseRelease, RightMouseRelease, MiddleMouseRelease,
//	KeyPress, KeyRelease,
//	WindowResize, WindowMove
//};
//
//using EventHandler = void(*)(Event* event);
//using EventType = int;
//
//std::unordered_map<EventType, EventHandler> handlers;

class IEventListener {
public:
};

template<typename T>
class EventListener : public IEventListener {
};

class IEventDelegate {
public:
	virtual void invoke(Event* event) = 0;
};

template<typename T>
class ClassEventDelegate : public IEventDelegate {
private:
	T* object;
	void(T::* f)(Event* event);
public:
	ClassEventDelegate(T* obj, void(T::*func)(Event* event)) : object(obj), f(func)
	{}

	virtual void invoke(Event* event) override {
		(object->*f)(event);
	}
};

template<typename T>
class EventDelegate : public IEventDelegate {
public:
	EventListener<T>* listener;
	void(T::*f)(Event* event);
public:
	EventDelegate(EventListener<T>* listener, void(T::*f)(Event* event)):
		listener(listener), f(f)
	{}

	virtual void invoke(Event* event) override {
		(((T*)listener)->*f)(event);
	}
};

class EventSystem {
private:
	Event** events = nullptr;
	int curSize = 0;
	std::unordered_map<size_t, std::vector<IEventDelegate*>> delegates;
public:
	EventSystem() {
		events = (Event**)malloc(1000 * sizeof(uintptr_t));
	}

	void addEvent(Event* event) {
		*(events + curSize++) = event;
	}

	template<typename T, typename EventType>
	void addEventListener(EventListener<T>* eventListener, void(T::* f)(Event* event)) {
		delegates[EventType::staticEventTypeId()].push_back(new EventDelegate(eventListener, f));
	}
	template<typename T, typename EventType>
	void addEventListener(IEventDelegate* eventDelegate) {
		delegates[EventType::staticEventTypeId()].push_back(eventDelegate);
	}

	void flushEvents() {
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
};