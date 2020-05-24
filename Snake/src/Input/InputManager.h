#pragma once

class InputManager {
private:
	bool keyboardKeys[256];
	bool mouseButton[5];

	static InputManager* _instance;
private:
	InputManager() {
		for (int i = 0; i < 256; i++) {
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

	bool isKeyPressed(int keyCode)
	{
		return keyboardKeys[keyCode];
	}

	bool isMouseKeyPressed(int mouseCode)
	{
		return mouseButton[mouseCode];
	}

	void setKeyPressed(int keyCode, bool state) {
		keyboardKeys[keyCode] = state;
	}
};