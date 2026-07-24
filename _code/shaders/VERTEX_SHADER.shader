#version 330 core
layout(location = 0) in float in_x;
layout(location = 1) in float in_y;
layout(location = 2) in float in_r;
layout(location = 3) in float in_g;
layout(location = 4) in float in_b;
layout(location = 5) in float in_u; //texture 
layout(location = 6) in float in_v; //texture 

uniform mat4 mvp;

out vec3 vColor;
out vec2 TexCoords; 

void main() {
    gl_Position = mvp * vec4(in_x, in_y, 0.0, 1.0);
    vColor = vec3(in_r, in_g, in_b);
    TexCoords = vec2(in_u, in_v); // Собираем vec2 текстурных координат
}