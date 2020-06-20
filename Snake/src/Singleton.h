#pragma once

template<typename T>
class Singleton {
private:
	static T* _instance;
public:
	template<typename ...ARGS>
	void startUp(ARGS&& ...args) {
		_instance = new T(std::forward(args)...);
	}

	static T* instance() { return _instance; }
};

template<typename T> T* Singleton<T>::_instance = nullptr;
