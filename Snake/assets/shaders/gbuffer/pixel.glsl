#version 460

layout(location = 0) out vec4 positions;
layout(location = 1) out vec4 normals;
layout(location = 2) out vec4 colors;

in VS_OUT
{
    vec4 position;
    vec2 texCoord;
    vec3 normal;
} fs_in;

uniform sampler2D diffuseTexture; //0
uniform sampler2D normalTexture; //1

void main() {
    positions = fs_in.position;
    normals = vec4(fs_in.normal, 1.0f);
    colors = texture(diffuseTexture, fs_in.texCoord);
}