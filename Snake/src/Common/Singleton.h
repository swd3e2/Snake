#pragma once

template<typename T>
class Singleton {
private:
	static T* _instance;
public:
	template<typename... ARGS>
	static T* startUp(ARGS&&... args) {
		_instance = new T(std::forward(args)...);
		return _instance;
	}

	static T* instance() { return _instance; }
protected:
	Singleton() = default;
	virtual ~Singleton() {}

	Singleton(const Singleton&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};

template<typename T> T* Singleton<T>::_instance = nullptr;
