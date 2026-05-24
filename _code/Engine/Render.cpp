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
    
    //----------------------------------> texture ------------------------------------->
    TEXTURE_SHADER->use(); 
    TEXTURE_SHADER->setMat4("mvp", p->mvp.mvp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, p->texture.texArray);
    // Позиция и Радиус
    //  ПЕРЕДАЕМ КООРДИНАТЫ (Массивы по 32 или 100 элементов)
    glUniform1fv(glGetUniformLocation(TEXTURE_SHADER->ID, "u_LX"), p->light->count, p->light->pos_X);
    glUniform1fv(glGetUniformLocation(TEXTURE_SHADER->ID, "u_LY"), p->light->count, p->light->pos_Y);
    glUniform1fv(glGetUniformLocation(TEXTURE_SHADER->ID, "u_LRadius"), p->light->count, p->light->radius);

    //  ПЕРЕДАЕМ ЦВЕТА (Три отдельных массива из  SoA структуры)
    glUniform1fv(glGetUniformLocation(TEXTURE_SHADER->ID, "u_LColorR"), p->light->count, p->light->color_R);
    glUniform1fv(glGetUniformLocation(TEXTURE_SHADER->ID, "u_LColorG"), p->light->count, p->light->color_G);
    glUniform1fv(glGetUniformLocation(TEXTURE_SHADER->ID, "u_LColorB"), p->light->count, p->light->color_B);

    //  ПЕРЕДАЕМ КОЛИЧЕСТВО (Чтобы шейдер знал, сколько итераций делать в цикле)
    glUniform1i(glGetUniformLocation(TEXTURE_SHADER->ID, "u_LightCount"), p->light->count);
    
    // Привязываем наш массив текстур 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, p->texture.texArray);

    //----------------------------------> block ------------------------------------->
    if ( p->block->count > 0 )
    {
       
        // Загружаем готовый батч из оперативной памяти (p->batch_ptr) в видеопамять (VBO)
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.texture_VBO);
        // Отправляем ровно столько данных, сколько реально насчитали в CPU (count * 20 флоатов)
        glBufferSubData(GL_ARRAY_BUFFER, 0, p->block->count * 20 * sizeof(float), p->batch_block_ptr);

        // Привязываем настройки атрибутов и индексы
        glBindVertexArray(shader_id.texture_VAO);

        // ОДИН ВЫЗОВ НА ВСЕ ОБЪЕКТЫ
        // Индексов в 6 раз больше, чем блоков
        glDrawElements(GL_TRIANGLES, p->block->count * 6, GL_UNSIGNED_INT, 0);
    }
    //----------------------------------> light next --------------------------------->
    if (p->light->count > 0)
    {
        // ВКЛЮЧАЕМ СМЕШИВАНИЕ Свет будет складываться с картинкой
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Режим "Добавление" (Linear Dodge)

        auto& LIGHT_SHADER = shader_id.light_shader;
        LIGHT_SHADER->use();
        LIGHT_SHADER->setMat4("mvp", p->mvp.mvp);

        // Загружаем световой батч (у него 32 float на лампу: 4 вершины * 8 параметров)
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.light_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, p->light->count * 32 * sizeof(float), p->batch_light_ptr);

        glBindVertexArray(shader_id.light_VAO);
        glDrawElements(GL_TRIANGLES, p->light->count * 6, GL_UNSIGNED_INT, 0);

       

        glDisable(GL_BLEND); // Выключаем чтобы не испортить следующий кадр
    }
    //----------------------------------> gui button next --------------------------------->
    if (p->gui_vertex->count > 0)
    {
      /*
        for (unsigned int i = 0; i < p->gui_vertex->count; i++) {
            std::cout << "Vert " << i << " -> X: " << p->gui_vertex->x[i] << " Y: " << p->gui_vertex->y[i] << "\n";
        }
        std::cout << "Total vertices: " << p->gui_vertex->count << "\n";
      */

        auto& BASED_SHADER = shader_id.based_shader;
        BASED_SHADER->use();
        BASED_SHADER->setMat4("mvp", p->mvp.gui_ortho);

        glBindVertexArray(shader_id.GUI_VAO);

        unsigned int v_count = p->gui_vertex->count;
        size_t data_size = v_count * sizeof(float);

        // ОБНОВЛЯЕМ БУФЕРЫ 
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_X_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, p->gui_vertex->x);

        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_Y_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, p->gui_vertex->y);

        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_R_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, p->gui_vertex->colorR);

        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_G_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, p->gui_vertex->colorG);

        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_B_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, p->gui_vertex->colorB);

      
        glDrawArrays(GL_TRIANGLES, 0, v_count);

        // СБРАСЫВАЕМ СТАТУС В 0 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    
     

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

    auto& LIGHT_SHADER = shader_id.light_shader;
    LIGHT_SHADER->use();
    LIGHT_SHADER->setMvp(current_mvp);


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

    //GUI VBO VAO \\ НЕ ШЕЙДЕР // -------------------------------------->
    
        glGenVertexArrays(1, &shader_id.GUI_VAO);
        glBindVertexArray(shader_id.GUI_VAO);

        //  Буфер для координат X
        glGenBuffers(1, &shader_id.GUI_X_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_X_VBO);
        // Выделяем память под максимум вершин (4096 * sizeof(float))
        glBufferData(GL_ARRAY_BUFFER, 4096 * sizeof(float), nullptr, GL_STREAM_DRAW);
        glEnableVertexAttribArray(0); // layout(location = 0) in float in_x;
        glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);

        //  Буфер для координат Y
        glGenBuffers(1, &shader_id.GUI_Y_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_Y_VBO);
        glBufferData(GL_ARRAY_BUFFER, 4096 * sizeof(float), nullptr, GL_STREAM_DRAW);
        glEnableVertexAttribArray(1); // layout(location = 1) in float in_y;
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);

        // Буфер для Color R
        glGenBuffers(1, &shader_id.GUI_R_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_R_VBO);
        glBufferData(GL_ARRAY_BUFFER, 4096 * sizeof(float), nullptr, GL_STREAM_DRAW);
        glEnableVertexAttribArray(2); // layout(location = 2) in float in_r;
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

        // Буфер для Color G
        glGenBuffers(1, &shader_id.GUI_G_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_G_VBO);
        glBufferData(GL_ARRAY_BUFFER, 4096 * sizeof(float), nullptr, GL_STREAM_DRAW);
        glEnableVertexAttribArray(3); // layout(location = 3) in float in_g;
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);

        // Буфер для Color B
        glGenBuffers(1, &shader_id.GUI_B_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, shader_id.GUI_B_VBO);
        glBufferData(GL_ARRAY_BUFFER, 4096 * sizeof(float), nullptr, GL_STREAM_DRAW);
        glEnableVertexAttribArray(4); // layout(location = 4) in float in_b;
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
        
       //-------------------------------------->
    
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
    {
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

      
    }
    shader_id.texture_shader = new Shader("_code/shaders/TexSHADER/VERT_SHADER.shader", "_code/shaders/TexSHADER/FRAG_SHADER.shader");

    shader_id.texture_shader->use();
    // привязать к униформе в шейдере
    glUniform1i(glGetUniformLocation(shader_id.texture_shader->ID, "u_TextureArray"), 0);

    glBindVertexArray(0); // Отключаемся

    //----------------------------------------------------------------->//
    glGenVertexArrays(1, &shader_id.light_VAO);
    glGenBuffers(1, &shader_id.light_VBO);
    glGenBuffers(1, &shader_id.light_EBO);

    glBindVertexArray(shader_id.light_VAO);

    // VBO для динамических данных света (8 float на вершину * 4 вершины * MAX_LIGHTS)
    glBindBuffer(GL_ARRAY_BUFFER, shader_id.light_VBO);
    glBufferData(GL_ARRAY_BUFFER, MAX_LIGHTS * 4 * 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    
    // EBO (те же индексы 0,1,3, 1,2,3... что и для блоков)
    {
        unsigned int light_indices[MAX_LIGHTS * 6];
        for (unsigned int i = 0; i < MAX_LIGHTS; i++) {
            unsigned int offset = i * 4;
            light_indices[i * 6 + 0] = offset + 0;
            light_indices[i * 6 + 1] = offset + 1;
            light_indices[i * 6 + 2] = offset + 3;
            light_indices[i * 6 + 3] = offset + 1;
            light_indices[i * 6 + 4] = offset + 2;
            light_indices[i * 6 + 5] = offset + 3;
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_id.light_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(light_indices), light_indices, GL_STATIC_DRAW);
    }
    // --- АТРИБУТЫ (Stride = 8 * 4 байта = 32 байта) ---
    GLsizei l_stride = 8 * sizeof(float);

    // 0: aPos (vec2) - Координаты углов "светового квадрата"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, l_stride, (void*)0);

    // 1: iCenter (vec2) - Центр лампы (для градиента)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, l_stride, (void*)(2 * sizeof(float)));

    // 2: iColor (vec3) - Цвет RGB
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, l_stride, (void*)(4 * sizeof(float)));

    // 3: iRadius (float) - Радиус затухания
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, l_stride, (void*)(7 * sizeof(float)));

    shader_id.light_shader = new Shader("_code/shaders/LightSHADER/VERT_SHADER.shader", "_code/shaders/LightSHADER/FRAG_SHADER.shader");

    glBindVertexArray(0);
    //-----------------------------------------------------------------//
    return;
}
