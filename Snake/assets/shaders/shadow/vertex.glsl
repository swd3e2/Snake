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
} vs_out;

out gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

void main() {
    vs_out.position = vec4(pos.x, pos.y, pos.z, 1.0f) * transform * toWorld;
    gl_Position = vs_out.position * WVP;
}