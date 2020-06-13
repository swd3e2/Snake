#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;

out gl_PerVertex {
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
};

out VS_OUT
{
    vec4 position;
    vec2 texCoord;
} vs_out;

void main() {
    vs_out.position = gl_Position = vec4(pos.x, pos.y, pos.z, 1.0f);
    vs_out.texCoord = texCoords;
}