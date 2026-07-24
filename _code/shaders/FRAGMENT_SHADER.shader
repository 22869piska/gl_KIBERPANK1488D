#version 330 core

in vec3 vColor;
in vec2 TexCoords;

uniform sampler2D fontTexture; //  сэмплер для текстуры шрифта [INDEX]

out vec4 FinalOutColor;

void main() {
    // Читаем пиксель из текстуры шрифта [INDEX]
    vec4 texColor = texture(fontTexture, TexCoords);
    
      if(texColor.a < 0.5) {
        discard; 
    }  
    // Умножаем цвет текстуры на цвет из батча
    FinalOutColor = texColor * vec4(vColor, 1.0);
    
  
}