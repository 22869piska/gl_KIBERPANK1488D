#version 330 core
layout (location = 0) in vec2 aPos; // Координаты из твоего Batch (уже со scale/rotate)
layout (location = 1) in vec2 aTex;
layout (location = 2) in float aID;

uniform mat4 mvp;

out vec2 v_WorldPos; // Передаем во фрагментный
out vec2 UV;
out float texID;

void main() {
    v_WorldPos = aPos; // Сохраняем "мировую" позицию точки
    gl_Position = mvp * vec4(aPos, 0.0, 1.0);
    UV = aTex;
    texID = aID;
}