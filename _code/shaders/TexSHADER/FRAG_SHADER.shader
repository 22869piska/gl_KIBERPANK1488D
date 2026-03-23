#version 330 core
out vec4 FragColor;

in vec2 v_WorldPos; 
in vec2 UV;
in float texID;

const int MAX_LIGHTS = 32; // Должно совпадать с твоим LightSystem
uniform sampler2DArray u_TextureArray;

// ПРИНИМАЕМ МАССИВЫ ИЗ C++
uniform int u_LightCount;
uniform float u_LX[MAX_LIGHTS]; 
uniform float u_LY[MAX_LIGHTS];
uniform float u_LRadius[MAX_LIGHTS];
uniform float u_LColorR[MAX_LIGHTS];
uniform float u_LColorG[MAX_LIGHTS];
uniform float u_LColorB[MAX_LIGHTS];

void main() {
    vec4 tex = texture(u_TextureArray, vec3(UV, texID));
    if(tex.a < 0.1) discard;

    vec3 ambient = vec3(0.00); // Почти полная темнота
    vec3 lightSum = vec3(0.0);

    for(int i = 0; i < u_LightCount; i++) {
        float dist = distance(v_WorldPos, vec2(u_LX[i], u_LY[i]));
        
        if(dist < u_LRadius[i]) {
            // Собираем цвет
            vec3 lColor = vec3(u_LColorR[i], u_LColorG[i], u_LColorB[i]);
            
            // Расчет затухания
            float atten = 1.0 - (dist / u_LRadius[i]);
            atten = clamp(atten, 0.0, 1.0);
            
           
            lightSum += lColor * atten; 
        }
    }

    FragColor = vec4(tex.rgb * (ambient + lightSum), tex.a);
}
   
