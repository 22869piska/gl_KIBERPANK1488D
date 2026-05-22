#include "engine.hpp"
#include "CALLBACK.hpp"
#include "../asm/asm.hpp"

#define RUNTIME_PROC ((LogicFunc)this->runtime_code)(this);
Engine* Engine::native_ptr = nullptr;

Engine::Engine() 
{
    this->native_ptr = this;

    macro_def();
    initGlContext();
    glDisable(GL_CULL_FACE);
    //texture.LoadAllTexture();
    texture.texArray = texture.LoadTextureArray();
    render = new (&render_mem)Render();
    render->p = this;

    memset(block_mem, 0, sizeof(Block));
    memset(light_mem, 0, sizeof(Light));
    
    block = new (&block_mem)Block();
    light = new (&light_mem)Light();

    

    init();
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
    this->gui_vertex = reinterpret_cast<GuiVertex*>(this->gui_vertex_mem);
    memcpy(runtime_code, ASM::RuntimeProc_START, ASM::RP_SIZE); //ASM КОД копироовать из .ASM В ПРОЦЕДУРУ РАНТАЙМА

    std::cout << "ASM OPCODES SIZE: " << ASM::RP_SIZE << " bytes \t [ MAX " <<this->code_size << " ] bytes\n";
    if (ASM::RP_SIZE > this->code_size) { std::cout << "!!!!!!!!!!!!!!!!ASM_CODE > MEMORY_ALLOC_SIZE!\n"; return; }

    printf("ADDR == THIS->BLOCK_BASE: %llu\n", (uintptr_t)&this->block_mem - (uintptr_t)this);
    printf("ADDR == THIS->BLOCK_LIGHT: %llu\n", (uintptr_t)&this->light_mem - (uintptr_t)this);
    printf("ADDR == THIS->ASM_MY_MEMORY: %llu\n", (uintptr_t)&this->MY_MEM - (uintptr_t)this);
}
void Engine::Run()
{
    while (!glfwWindowShouldClose(window.window))
    {

        if (isRunning == false) { SaveLevel(); }
        gui.MenuManager();
        gui_vertex->x[90] = 1;
       
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
    if (this->isRunning == true)
    {
        RUNTIME_PROC
    }
    
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
  const char* path = "temp/save.bin";

    if (glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS) { //save level
       
        // Все OBJ
        BIN_IO::Save(this->block, sizeof(Block), path); // Записали блок (создали файл)
        BIN_IO::SaveAppend(this->light, sizeof(Light), path); // Дописали свет в конец

        //CODE  asm
        BIN_IO::SaveAppend(this->MY_MEM, sizeof(this->MY_MEM), path); //MY_MEM
        BIN_IO::SaveAppend(this->code_ptr, this->code_size, path); //Дaльше code дописываем
    }
    if (glfwGetKey(window.window, GLFW_KEY_L) == GLFW_PRESS) { // load level
  
        size_t offset = 0; 

        //Блоки
        BIN_IO::Load(this->block, sizeof(Block), path, offset);
        offset += sizeof(Block); 

        //Свет
        BIN_IO::Load(this->light, sizeof(Light), path, offset);
        offset += sizeof(Light); 

        //MY_MEM
        BIN_IO::Load(MY_MEM, sizeof(this->MY_MEM), path, offset);
        offset += sizeof(this->MY_MEM);

        //CODE
        BIN_IO::Load(this->code_ptr, this->code_size, path, offset);

        this->isRunning = true;

        for (int i = 0;i < sizeof(this->MY_MEM);i++)
        {
            std::cout << std::bitset<8>(MY_MEM[i]);std::cout << "|>>|";

        }
     
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
    InitCallBack();
   
} 
void Engine::InitCallBack()
{
    glfwSetFramebufferSizeCallback(this->window.window, Callback::FramebufferSize);
    glfwSetCursorPosCallback(this->window.window, Callback::CursorPosCallback);
}
//-------------------------------------------//