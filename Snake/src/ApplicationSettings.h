#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

class ApplicationSettings 
{
private:
	int windowHeight = 1080;
	int windowWidth = 1920;

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

	inline int getWindowWidth() { return windowWidth; }
	inline int getWindowHeight() { return windowHeight; }

	inline int getShadowMapWidth() { return shadowMapWidth; }
	inline int getShadowMapHeight() { return shadowMapHeight; }
};