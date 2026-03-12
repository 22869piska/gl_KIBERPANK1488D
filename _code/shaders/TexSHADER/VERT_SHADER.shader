#version 330 core
layout (location = 0) in vec2 aPos; // Уже готовая позиция
layout (location = 1) in vec2 aTex;
layout (location = 2) in float aID;

out vec2 UV;
out float texID;

uniform mat4 mvp; //только view * proj

void main() {
    gl_Position = mvp * vec4(aPos, 0.0, 1.0);
    UV = aTex;
    texID = aID;
}