#pragma once

class InputManager {
public:
	double mousePosX = 0.0;
	double mousePosY = 0.0;

	double mouseMoveX = 0.0;
	double mouseMoveY = 0.0;
private:
	bool keyboardKeys[349];
	bool mouseButton[5];

	static InputManager* _instance;
private:
	InputManager() {
		for (int i = 0; i < 348; i++) {
			keyboardKeys[i] = false;
		}
		for (int i = 0; i < 5; i++) {
			mouseButton[i] = false;
		}
	}
public:
	static InputManager* instance() {
		if (_instance == nullptr) {
			_instance = new InputManager();
		}
		return _instance;
	}

	inline bool isKeyPressed(int keyCode) { return keyboardKeys[keyCode]; }
	inline void setKeyPressed(int keyCode, bool state) { keyboardKeys[keyCode] = state; }
	inline bool isMouseKeyPressed(int mouseCode) { return mouseButton[mouseCode]; }
	inline void setMouseKeyPressed(int mouseCode, bool state) { mouseButton[mouseCode] = state; }

	void setCursorPosition(double x, double y) {
		mouseMoveX = x - mousePosX;
		mouseMoveY = y - mousePosY;

		mousePosX = x;
		mousePosY = y;
	}
};