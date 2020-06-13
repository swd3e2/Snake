#version 460

out vec4 FragColor;

in VS_OUT
{
    vec4 position;
} fs_in;

void main() {
    FragColor = vec4(gl_FragCoord.z);
}