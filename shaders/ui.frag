#version 330 core
out vec4 FragColor;

// A simple uniform to set the color from C++
uniform vec4 color;

void main() {
    FragColor = color;
}