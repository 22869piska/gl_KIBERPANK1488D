#include "Render.hpp"
#include "Engine.hpp"

Render::Render()
{
    InitShaders();
}
Render::~Render()
{
    
}
void Render::GPU()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SpaceView();

    // --- ОТРИСОВКА БАТЧА (Текстурные объекты) ---
    auto& TEXTURE_SHADER = shader_id.texture_shader;
    TEXTURE_SHADER->use(); // На всякий случай включаем еще раз

    // Привязываем наш массив текстур 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, p->texture.texArray);

    // Загружаем готовый батч из оперативной памяти (p->batch_ptr) в видеопамять (VBO)
    glBindBuffer(GL_ARRAY_BUFFER, shader_id.texture_VBO);
    // Отправляем ровно столько данных, сколько реально насчитали в CPU (count * 20 флоатов)
    glBufferSubData(GL_ARRAY_BUFFER, 0, p->block->count * 20 * sizeof(float), p->batch_ptr);

    // Привязываем настройки атрибутов и индексы
    glBindVertexArray(shader_id.texture_VAO);

    // ОДИН ВЫЗОВ НА ВСЕ ОБЪЕКТЫ
    // Индексов в 6 раз больше, чем блоков
    glDrawElements(GL_TRIANGLES, p->block->count * 6, GL_UNSIGNED_INT, 0);

    // --- ОТРИСОВКА ПРОСТЫХ ТРЕУГОЛЬНИКОВ (если они есть) ---
    // shader_id.based_shader->use();
    // glBindVertexArray(shader_id.VAO_triangle);
    // glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}
//private ->
void Render::SpaceView()
{
    const glm::mat4& current_mvp = p->mvp.mvp;
    // для вдиеокарты GPU_LOOP:
    
    auto& BASE_SHADER = shader_id.based_shader;
    BASE_SHADER->use();
    BASE_SHADER->setMat4("mvp", current_mvp);

    auto& TEXTURE_SHADER = shader_id.texture_shader;
    TEXTURE_SHADER->use();
    TEXTURE_SHADER->setMvp(current_mvp);


}
void Render::InitShaders()
{
    
    //3ygolniki->
    glGenVertexArrays(1, &shader_id.VAO_triangle);
    glGenBuffers(1, &shader_id.VBO_triangle);
    glGenBuffers(1, &shader_id.EBO);
    //
    glBindVertexArray(shader_id.VAO_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, shader_id.VBO_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex.triangle), vertex.triangle, GL_STATIC_DRAW);
    //-kubiki->
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_id.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertex.indices), vertex.indices, GL_STATIC_DRAW);
    //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //--//
    
    shader_id.based_shader = new Shader("_code/shaders/VERTEX_SHADER.shader", "_code/shaders/FRAGMENT_SHADER.shader");
    
    //next shader->
    //---------------------------------//
    //---------------------------------//
    // --- Шейдер для БАТЧИНГА СПРАЙТОВ ---
    // 1. Генерируем всё
    glGenVertexArrays(1, &shader_id.texture_VAO);
    glGenBuffers(1, &shader_id.texture_VBO);
    glGenBuffers(1, &shader_id.texture_EBO);

    // 2. СНАЧАЛА ПРИВЯЗЫВАЕМ VAO
    glBindVertexArray(shader_id.texture_VAO);

    // 3. ТЕПЕРЬ ПРИВЯЗЫВАЕМ И ЗАПОЛНЯЕМ БУФЕРЫ (они запишутся в этот VAO)
    glBindBuffer(GL_ARRAY_BUFFER, shader_id.texture_VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_BLOCK * 20 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Заполняем индексы внутрь VAO
    unsigned int all_indices[MAX_BLOCK * 6];
    for (unsigned int i = 0; i < MAX_BLOCK; i++) {
        unsigned int offset = i * 4;
        all_indices[i * 6 + 0] = offset + 0;
        all_indices[i * 6 + 1] = offset + 1;
        all_indices[i * 6 + 2] = offset + 3;
        all_indices[i * 6 + 3] = offset + 1;
        all_indices[i * 6 + 4] = offset + 2;
        all_indices[i * 6 + 5] = offset + 3;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_id.texture_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(all_indices), all_indices, GL_STATIC_DRAW);

    // 4. НАСТРАИВАЕМ АТРИБУТЫ (пока VAO привязан)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));
    glEnableVertexAttribArray(2);

    shader_id.texture_shader = new Shader("_code/shaders/TexSHADER/VERT_SHADER.shader", "_code/shaders/TexSHADER/FRAG_SHADER.shader");

    glBindVertexArray(0); // Отключаемся
    //-----------------------------------------------------------------//
    return;
}
