#version 330 core
out vec4 FragColor;

in vec2 UV;
flat in float texID; // flat, чтобы индекс не "размывался" между вершинами

uniform sampler2DArray u_TextureArray; // Наш стек текстур

void main() {
    // texture() для массива требует vec3(u, v, layer)

    FragColor = texture(u_TextureArray, vec3(UV, texID));
    //FragColor = vec4(1.f,0.2f,0.3f,1.0f);
}
