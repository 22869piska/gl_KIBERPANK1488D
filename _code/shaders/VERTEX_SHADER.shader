#version 330 core

layout(location = 0) in float in_x;
layout(location = 1) in float in_y;
layout(location = 2) in float in_r;
layout(location = 3) in float in_g;
layout(location = 4) in float in_b;

uniform mat4 mvp;

out vec3 FRGcolor; // Передаем цвет во фрагментный шейдер

void main() {
    // Собираем позицию и цвет прямо в чипе видеокарты!
    gl_Position = mvp * vec4(in_x, in_y, 0.0, 1.0);
    
    FRGcolor = vec3(in_r, in_g, in_b);
}