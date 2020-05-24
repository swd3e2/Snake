#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>

class ShaderPipeline
{
public:
	unsigned int vertexShaderId;
	unsigned int pixelShaderId;
	unsigned int shaderPipelineId;
public:
    ShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode)
	{
        const char* temp = vertexShaderCode.c_str();
        vertexShaderId = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, &temp);

        int isLinked = true;
        glGetProgramiv(vertexShaderId, GL_LINK_STATUS, &isLinked);

        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(vertexShaderId, GL_INFO_LOG_LENGTH, &maxLength);
            GLchar* infoLog = new GLchar[maxLength];
            glGetProgramInfoLog(vertexShaderId, maxLength, &maxLength, infoLog);
            std::cout << infoLog;
            delete[] infoLog;
            return;
        }

        temp = pixelShaderCode.c_str();
        pixelShaderId = glCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &temp);

        glGetProgramiv(pixelShaderId, GL_LINK_STATUS, &isLinked);

        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(pixelShaderId, GL_INFO_LOG_LENGTH, &maxLength);
            GLchar* infoLog = new GLchar[maxLength];
            glGetProgramInfoLog(pixelShaderId, maxLength, &maxLength, infoLog);
            std::cout << infoLog;
            delete[] infoLog;
        }

        std::unordered_map<std::string, int> textures;
        int size = 0;

        {
            const char* charShaderCode = pixelShaderCode.c_str();
            const char* entrance = strstr(charShaderCode, "uniform sampler2D");
            const char* temp;
            while (entrance != nullptr)
            {
                entrance += strlen("uniform sampler2D") + 1; // skip "uniform sampler2D" and space
                temp = strstr(entrance, ";");
                size = temp - entrance;
                char* uniformName = new char[size + 1];
                strncpy(uniformName, entrance, size);
                uniformName[size] = '\0';

                entrance = strstr(entrance, "//");
                entrance += strlen("//");
                temp = strstr(entrance, "\n");
                size = temp - entrance;
                char* slot = new char[size + 1];
                strncpy(slot, entrance, size);
                slot[size] = '\0';

                textures[uniformName] = atoi(slot);
                entrance = strstr(entrance, "uniform sampler2D");
            }
        }
        for (auto& it : textures) {
            glProgramUniform1i(pixelShaderId, glGetUniformLocation(pixelShaderId, it.first.c_str()), it.second);
        }

        glGenProgramPipelines(1, &shaderPipelineId);
        glUseProgramStages(shaderPipelineId, GL_VERTEX_SHADER_BIT, vertexShaderId);
        glUseProgramStages(shaderPipelineId, GL_FRAGMENT_SHADER_BIT, pixelShaderId);
	}

    void bind()
    {
        glBindProgramPipeline(shaderPipelineId);
    }
};