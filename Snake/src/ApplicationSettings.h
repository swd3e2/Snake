#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <string>
#include "Common/Helper.h"

class ApplicationSettings 
{
private:
	int windowHeight = 1440;
	int windowWidth = 2560;

	int shadowMapWidth = 1024;
	int shadowMapHeight = 1024;

	bool ambientOcclusionEnabled = false;

	std::string runtimeDir;
	std::string assetsDir;
public:
	ApplicationSettings()
	{
		//Save runtime directory	
		char buffer[1000];
		GetModuleFileName(NULL, buffer, 1000);
		std::string::size_type pos = std::string(buffer).find_last_of("\\/");
		std::string temp = replace(std::string(buffer).substr(0, pos), "\\", "/");

		runtimeDir = temp;
		assetsDir = temp;
	}

	inline int getWindowWidth() const { return windowWidth; }
	inline int getWindowHeight() const { return windowHeight; }

	inline int getShadowMapWidth() const { return shadowMapWidth; }
	inline int getShadowMapHeight() const { return shadowMapHeight; }
};