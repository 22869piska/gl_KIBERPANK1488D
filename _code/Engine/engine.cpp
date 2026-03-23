#include "engine.hpp"


Engine::Engine() 
{
    init();
    //texture.LoadAllTexture();
    texture.texArray = texture.LoadTextureArray();
    render = new (&render_mem)Render();
    render->p = this;

    memset(block_mem, 0, sizeof(Block));
    memset(light_mem, 0, sizeof(Light));
    
    block = new (&block_mem)Block();
    light = new (&light_mem)Light();

}
Engine::~Engine()
{
    render->~Render();
    block->~Block();
    light->~Light();

    timeBeginPeriod(0);
}
//процедуры ->
void Engine::init()
{
    //initGLContext();
    macro_def();
    initGlContext();
    glDisable(GL_CULL_FACE);
}
void Engine::Run()
{
    while (!glfwWindowShouldClose(window.window))
    {

        //IO::Input();
        CPU();
        this->render->GPU();
        Tps();
        glfwSwapBuffers(window.window);
        glfwPollEvents();
    }
   
}
//-------------------------------------------//
void Engine::CPU()
{

    unsigned long long clock = ASM::ClockCounter();
    //;-------------------------------------------;//
    //ЛОГИКА ИГРЫ ну типа

    code_ptr[0] = 0xC3;

    typedef void (*LogicFunc)();

    // Кастуем адрес и вызываем его
    if (this->runtime_code != nullptr) {
        ((LogicFunc)this->runtime_code)();
    }




    /*
    param.pos = glm::vec2(-300.f,-300.f);
    param.scale = glm::vec2(100.f,100.f);
    param.tex = texture.VENTILATOR_2;
    for (unsigned int i = block->count;i < 30;i++)
    {
        this->block->add(param);
        param.pos.y += 30.0f;
        param.pos.x += 30.0f;

        param.tex = param.tex ^ (texture.VENTILATOR ^ texture.VENTILATOR_2);
    }

    for (unsigned int i = light->count; i < 2; i++)
    {
        light->add(0, -100.f, 0.0f, 0.0f, 1.0f, 500.0f, 1.0f);
    }
    */
    static float dir = 1.0f; 
    light->pos_X[0] += 7.0f * dir;
    light->color_G[0] = 1.f;
    light->color_R[0] = glm::sin(glfwGetTime());
    light->color_B[0] = 0.1F * light->color_G[0];

    light->radius[0] = 200.21f;

    if (light->pos_X[0] >= 500.0f) {
        dir = -1.f; 
    }
    if (light->pos_X[0] <= -500.0f) {
        dir = 1.f;  
    }

    //---------------------------------------------//
    
    SaveLevel();

    //;-------------------------------------------;//
     // ПОДГОТОВКА ЗНАЧЕНИЙ ДЛЯ GPU 
    Batch();

    clock = ASM::ClockCounter() - clock;
    //std::cout <<" clock_per_frame_CPU -> " << clock << " \n";
}
//-------------------------------------------//
//private ->
void Engine::SpaceView()
{
    mvp.projection = glm::ortho(
        -window.width / 2.0f, window.width / 2.0f,
        -window.height / 2.0f, window.height / 2.0f,
        -100.0f, 100.0f  
    );
    mvp.view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 10.0f),  // ïîçèöèÿ êàìåðû
        glm::vec3(0.0f, 0.0f, 0.0f),  // öåëü êàìåðû
        glm::vec3(0.0f, 1.0f, 0.0f)   // âåêòîð "ââåðõ"
    );
    mvp.mvp = mvp.projection * mvp.view;
}
void Engine::Batch()
{
    SpaceView();
    Block_batch();
    Light_batch();
  
}
void Engine::Block_batch()
{
    //ВЫЧИСЛЫНИЕ И ЗАПОЛНЕНИЕ БУФЕРА ЧТОБ ГПУ РИСОВАЛ ПОТОМ
    static const unsigned int X_indx = 0;
    static const unsigned int Y_indx = 1;
    static const unsigned int U_indx = 2;
    static const unsigned int V_indx = 3;
    static const unsigned int ID_indx = 4;
    static const unsigned int STRIDE_F = 5;

    for (unsigned int i = 0; i < block->count; i++)//каждая вершина  это 5 float (x y u v ,ID)
    {
        unsigned int offset = i * 20;             //суммарное смещение буфера (в нем pos scale rotate texture и тд)

        float pos_X = this->block->pos_X[i];
        float pos_Y = this->block->pos_Y[i];
        float scale_X = this->block->scale_X[i];
        float scale_Y = this->block->scale_Y[i];
        float rotate_A = this->block->rotate_A[i];
        unsigned int texture_ID = this->block->texture[i];

        for (unsigned int v = 0; v < 4; v++)
        {
            unsigned int v_off_idx = offset + (v * STRIDE_F);
            unsigned int v_off = v * 5;

            float local_x = this->render->vertex.square_tex[v_off + 0] * scale_X;
            float local_y = this->render->vertex.square_tex[v_off + 1] * scale_Y;
            // pos x y
            this->batch_block_ptr[v_off_idx + X_indx] = local_x + pos_X;
            this->batch_block_ptr[v_off_idx + Y_indx] = local_y + pos_Y;
            // tex u v
            this->batch_block_ptr[v_off_idx + U_indx] = this->render->vertex.square_tex[v_off + 3];
            this->batch_block_ptr[v_off_idx + V_indx] = this->render->vertex.square_tex[v_off + 4];

            this->batch_block_ptr[v_off_idx + ID_indx] = texture_ID;
        }
    }
}
void Engine::Light_batch()
{
    float* b_ptr = this->batch_light_ptr; // Указатель на массив для GPU

    for (unsigned int i = 0; i < light->count; i++) {
        // Данные из твоей SoA структуры Light
        float cx = light->pos_X[i];
        float cy = light->pos_Y[i];
        float r  = light->radius[i];
        float ir = light->color_R[i];
        float ig = light->color_G[i];
        float ib = light->color_B[i];
        float intensity = light->intensity[i];

        // 4 вершины квадрата, который накроет область света
        for (unsigned int v = 0; v < 4; v++) {
            int v_off = v * 5; // Смещение в твоем шаблоне square_tex (x,y,z,u,v)

            // 1. Позиция угла (aPos) - расширяем квадрат до размеров радиуса
            // Используем шаблон (-0.5 .. 0.5) и множим на радиус * 2
            *b_ptr++ = (this->render->vertex.square_tex[v_off + 0] * r * 2.0f) + cx; // X
            *b_ptr++ = (this->render->vertex.square_tex[v_off + 1] * r * 2.0f) + cy; // Y

            // 2. Центр света (iCenter) - нужен шейдеру для расчета дистанции
            *b_ptr++ = cx;
            *b_ptr++ = cy;

            // 3. Цвет (RGB) с учетом интенсивности
            *b_ptr++ = ir * intensity;
            *b_ptr++ = ig * intensity;
            *b_ptr++ = ib * intensity;

            // 4. Радиус
            *b_ptr++ = r;
        }
    }
}
void Engine::Tps()
{
    static const double TARGET_TPS = 100.0;
    static const double TICK_TIME = 1.0 / TARGET_TPS; 
    static double last_tick_time = glfwGetTime();

    double current_time = glfwGetTime();
    double time_passed = current_time - last_tick_time;
    double time_to_wait = TICK_TIME - time_passed;

    if (time_to_wait > 0)
    {
        if (time_to_wait > 0.002)
        {
            auto sleep_duration = std::chrono::microseconds(int((time_to_wait - 0.002) * 1000000));
            std::this_thread::sleep_for(sleep_duration);
        }
        while (glfwGetTime() - last_tick_time < TICK_TIME)
        {
            std::this_thread::yield();
        }
    }
    last_tick_time = glfwGetTime();
    // --- Логгер TPS (автономный) ---
    static double log_timer = 0;
    static int ticks = 0;
    ticks++;
    if (glfwGetTime() - log_timer >= 1.0) {
        std::cout << ticks << " TPS ->\n";
        ticks = 0;
        log_timer = glfwGetTime();
    }
}
void Engine::SaveLevel()
{

    if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS) {
        const char* path = "temp/save.bin";
      
        BIN_IO::Save(this->block, sizeof(Block), path); // Записали блок (создали файл)
        BIN_IO::SaveAppend(this->light, sizeof(Light), path); // Дописали свет в конец
    }

    if (glfwGetKey(window.window, GLFW_KEY_L) == GLFW_PRESS) {
        const char* path = "temp/save.bin";
        // Читаем строго по своим местам!
        BIN_IO::Load(this->block, sizeof(Block), path,0); // Смещение 0
        BIN_IO::Load(this->light, sizeof(Light), path, sizeof(Block) ); // Смещение после блока
    }
}
//-------------------------------------------//
//init ->
//-------------------------------------------//
void Engine::macro_def()
{
    std::setlocale(LC_ALL, "Rus");
    timeBeginPeriod(1);
}
void Engine::initGlContext()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window.window = glfwCreateWindow(window.width, window.height, "openASSglfw", NULL, NULL);
    if (window.window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    
    //glfwSetFramebufferSizeCallback(gl_cd.window, framebuffer_size_callback);
    //glfwSetKeyCallback(gl_cd.window, key_callback);

} 
//-------------------------------------------//