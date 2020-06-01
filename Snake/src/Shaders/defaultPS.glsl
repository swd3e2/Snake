#version 460

out vec4 FragColor;

in VS_OUT
{
    vec4 position;
    vec2 texCoord;
    vec3 normal;
} fs_in;

layout(std140, binding = 3) uniform Material
{
    vec4 color;
};

uniform sampler2D diffuseTexture; //0
uniform sampler2D normalTexture;  //1

void main()
{
    vec4 texColor = texture(diffuseTexture, fs_in.texCoord);
    FragColor = color + clamp(dot(vec3(1.0f, 0.0f, 0.0f), fs_in.normal), 0.0f, 1.0f) * vec4(0.3f, 0.3f, 0.3f, 1.0f);
}