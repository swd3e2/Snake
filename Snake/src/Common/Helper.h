#pragma once

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

std::vector<std::string> split(std::string str, char ch);

std::string trimToLastLineEntry(const char* string, char entry);

std::string replace(const std::string& str, const std::string& find, const std::string& replace);

inline float lerp(float a, float b, float f) { return a + f * (b - a); }