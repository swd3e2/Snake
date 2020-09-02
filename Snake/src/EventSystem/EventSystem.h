#pragma once

#include <memory>
#include "Events.h"
#include <vector>
#include <unordered_map>

class IEventListener {};

template<typename T>
class EventListener : public IEventListener {};

class IEventDelegate 
{
public:
	virtual void invoke(Event* event) = 0;
};

template<typename T>
class ClassEventDelegate : public IEventDelegate 
{
private:
	T* object;
	void(T::* f)(Event* event);
public:
	ClassEventDelegate(T* obj, void(T::*func)(Event* event)) : object(obj), f(func) {}

	virtual void invoke(Event* event) override {
		(object->*f)(event);
	}
};

template<typename T>
class EventDelegate : public IEventDelegate 
{
public:
	EventListener<T>* listener;
	void(T::*f)(Event* event);
public:
	EventDelegate(EventListener<T>* listener, void(T::*f)(Event* event)) : listener(listener), f(f) {}

	virtual void invoke(Event* event) override {
		(((T*)listener)->*f)(event);
	}
};

class EventSystem 
{
private:
	Event** events = nullptr;
	int curSize = 0;
	std::unordered_map<size_t, std::vector<IEventDelegate*>> delegates;

	static EventSystem* _instance;
public:
	EventSystem();
	
	inline static EventSystem* instance() { return _instance; }
	
	void add(Event* event);

	template<typename T, typename EventType>
	void addEventListener(EventListener<T>* eventListener, void(T::* f)(Event* event)) 
	{
		delegates[EventType::staticEventTypeId()].push_back(new EventDelegate(eventListener, f));
	}

	template<typename T, typename EventType>
	void addEventListener(IEventDelegate* eventDelegate) 
	{
		delegates[EventType::staticEventTypeId()].push_back(eventDelegate);
	}

	void flush();
};