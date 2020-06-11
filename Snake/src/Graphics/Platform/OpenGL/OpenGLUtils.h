#pragma once

#include "Graphics/Renderer/CommonTypes.h"
#include <GL/glew.h>

GLuint getTextureFormat(TextureFormat format);
GLuint getInternalTextureFormat(TextureFormat format);
GLuint getInternalType(TextureFormat format);