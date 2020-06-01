#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;

layout(std140, binding = 0) uniform Matrices
{
    mat4 WVP;
};

layout(std140, binding = 1) uniform Model
{
    mat4 toWorld;
    mat4 inverseToWorld;
};

layout(std140, binding = 2) uniform Mesh
{
    mat4 transform;
    mat4 matrixTransform;
};

out VS_OUT
{
    vec4 position;
    vec2 texCoord;
    vec3 normal;
} vs_out;

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

void main() {
    gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f) * transform * toWorld * WVP;
    vs_out.normal = normalize(vec3(inverseToWorld * matrixTransform * vec4(normalize(normal), 0.0f)));
    vs_out.texCoord = texCoords;
    return;
}