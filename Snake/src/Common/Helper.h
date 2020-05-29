#pragma once

#include <string>

class Helper {
public:
	inline static std::string trimToLastLineEntry(const char* string, char entry)
	{
		std::string result;

		int lastEntry = -1;
		int stringLenght = strlen(string);

		for (int i = 0; i < stringLenght; i++) {
			if (string[i] == entry) {
				lastEntry = i;
			}
		}

		if (lastEntry == -1) {
			return result;
		}

		char* new_string = new char[lastEntry + 2];
		memcpy(new_string, string, lastEntry + 1);
		new_string[lastEntry + 1] = '\0';

		result = new_string;
		delete[] new_string;

		return result;
	}
};