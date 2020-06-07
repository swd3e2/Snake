#include "OpenGLShaderPipeline.h"

OpenGLShaderPipeline::OpenGLShaderPipeline(const std::string& vertexShaderCode, const std::string& pixelShaderCode) :
    ShaderPipeline(vertexShaderCode, pixelShaderCode) 
{
    const char* temp =  vertexShaderCode.c_str();
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
    std::vector<std::string> lines = split(pixelShaderCode, '\n');

    for (auto& line : lines) {
        if (line.rfind("uniform sampler2D") == 0) {
            std::vector<std::string> words = split(line, ' ');

            std::string textureUniformName = words[2];
            textureUniformName.erase(std::remove(textureUniformName.begin(), textureUniformName.end(), ';'), textureUniformName.end());

            std::string location = words[3];
            location.erase(std::remove(location.begin(), location.end(), '/'), location.end());

            textures[textureUniformName] = std::atoi(location.c_str());
        }
    }
    
    for (auto& it : textures) {
        glProgramUniform1i(pixelShaderId, glGetUniformLocation(pixelShaderId, it.first.c_str()), it.second);
    }

    glGenProgramPipelines(1, &shaderPipelineId);
    glUseProgramStages(shaderPipelineId, GL_VERTEX_SHADER_BIT, vertexShaderId);
    glUseProgramStages(shaderPipelineId, GL_FRAGMENT_SHADER_BIT, pixelShaderId);
}

void OpenGLShaderPipeline::bind(RenderContext* context)
{
    glBindProgramPipeline(shaderPipelineId);
}