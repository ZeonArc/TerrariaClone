#version 330 core
layout (location = 0) in vec2 aPos;

out vec2 ScreenPos;

void main() {
    ScreenPos = aPos;
    // Z is 1.0 so it sits at the absolute back of the screen
    gl_Position = vec4(aPos.x, aPos.y, 1.0, 1.0); 
}