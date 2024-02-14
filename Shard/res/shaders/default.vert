#version 420

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 u_MVP;

out vec3 normal_;
out vec2 texCoord_;

void main() {
    gl_Position = u_MVP * vec4(position, 1.0);
    normal_ = normal;
    texCoord_ = texCoord;
}