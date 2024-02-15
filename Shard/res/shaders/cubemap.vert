#version 420

layout (location = 0) in vec3 originalPos;

out vec3 TexCoords;

uniform mat4 u_MVP;

void main()
{
    TexCoords = originalPos;
    vec4 new_pos = u_MVP * vec4(originalPos, 1.0);
    gl_Position = new_pos.xyww;
}  