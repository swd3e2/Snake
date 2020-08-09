#pragma once

class InputManager {
public:
	double mousePosX = 0.0;
	double mousePosY = 0.0;

	double mouseMoveX = 0.0;
	double mouseMoveY = 0.0;
	int mouseMoveCnt = 0;
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
		mouseMoveCnt++;

		if (mouseMoveCnt == 1) {
			mouseMoveX += x - mousePosX;
			mouseMoveY += y - mousePosY;
		} else {
			mouseMoveX += (x - mousePosX) * 0.2f;
			mouseMoveY += (y - mousePosY) * 0.2f;
		}
		
		mousePosX = x;
		mousePosY = y;
	}
};