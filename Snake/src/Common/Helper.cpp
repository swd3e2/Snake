#include "Helper.h"

std::string trimToLastLineEntry(const char* string, char entry) {
    std::string result;

    int lastEntry = -1;
    size_t stringLenght = strlen(string);

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

std::vector<std::string> split(std::string str, char ch) {
	std::vector<std::string> lines;
	std::stringstream ss(str);
	
	std::string temp;
	while (std::getline(ss, temp, ch)) {
		lines.push_back(temp);
	}
    
	return lines;
}

std::string replace(const std::string& str, const std::string& find, const std::string& replace) {
    std::string copy(str);
	std::string::size_type position = 0;
	while ((position = copy.find(find)) != copy.npos) {
        copy.replace(position, 1, replace);
	}
    return copy;
}
