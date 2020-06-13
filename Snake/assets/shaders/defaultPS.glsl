#version 460

out vec4 FragColor;

uniform sampler2D diffuseTexture; //0
uniform sampler2D normalTexture; //1

void main()
{
    vec2 texSize = textureSize(diffuseTexture, 0).xy;
    vec2 texCoord = gl_FragCoord.xy / texSize;
    FragColor = texture(diffuseTexture, texCoord);
}