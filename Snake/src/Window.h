#pragma once

class Window {
private:
	int width = 0;
	int height = 0;
public:
	Window(int width, int height) :
		width(width), height(height)
	{}

	virtual bool isOpen() = 0;
	virtual void pollEvents() = 0;
};