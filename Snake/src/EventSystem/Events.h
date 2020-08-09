#pragma once

class Event {
public:
	virtual ~Event() {}

	static size_t staticEventTypeId() {
		return 0;
	}

	virtual size_t getTypeId() = 0;
};


class LeftMouseClickEvent : public Event {
public:
	static size_t staticEventTypeId() {
		return 1;
	}

	virtual size_t getTypeId() override {
		return staticEventTypeId();
	}
};
