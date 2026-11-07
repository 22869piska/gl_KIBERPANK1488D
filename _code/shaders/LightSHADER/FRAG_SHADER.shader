#version 330 core
out vec4 FragColor;

in vec2 v_FragPos;
in vec2 v_Center;
in vec3 v_Color;
in float v_Rad;

void main() {
    /*float dist = distance(v_FragPos, v_Center);
    
    // Если пиксель дальше радиуса - отсекаем (discard для скорости)
    if (dist > v_Rad) discard;

    // Мягкое затухание (инверсия дистанции)
    float attenuation = 1.0 - (dist / v_Rad);
    
    // Квадратичное затухание для более "физичного" вида
    attenuation *= attenuation;

    FragColor = vec4(v_Color, attenuation); 
    */
    vec2  d  = v_FragPos - v_Center;
    float t  = 1.0 - dot(d, d) / (v_Rad * v_Rad);   // без sqrt
    t = max(t, 0.0);
    FragColor = vec4(v_Color * (t * t), 1.0);       // вместе с glBlendFunc(GL_ONE, GL_ONE)
}