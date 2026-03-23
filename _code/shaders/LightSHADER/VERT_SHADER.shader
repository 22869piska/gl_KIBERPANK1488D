#version 330 core
layout (location = 0) in vec2 aPos;   // Координаты угла (уже вычислены на CPU)
layout (location = 1) in vec2 iCenter;// Центр этой лампы (передаем в батче)
layout (location = 2) in vec3 iColor; // Цвет лампы
layout (location = 3) in float iRad;  // Радиус

out vec2 v_FragPos;  // Позиция конкретного пикселя
out vec2 v_Center;   // Центр для расчета дистанции
out vec3 v_Color;
out float v_Rad;

uniform mat4 mvp;

void main() {
    v_FragPos = aPos;
    v_Center = iCenter;
    v_Color = iColor;
    v_Rad = iRad;
    
    gl_Position = mvp * vec4(aPos, 0.0, 1.0);
}