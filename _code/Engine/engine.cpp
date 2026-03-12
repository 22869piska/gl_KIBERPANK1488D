#include "engine.hpp"

Engine::Engine() 
{
   
    init();
    //texture.LoadAllTexture();
    texture.texArray = texture.LoadTextureArray();
    
    render = new (&render_mem) Render();
    render->p = this;
    block = new (&block_mem) Block();

}
Engine::~Engine()
{
    render->~Render();
    block->~Block();

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

    param.pos = glm::vec2(-300.f,-300.f);
    param.scale = glm::vec2(100.f,100.f);
    param.tex = texture.VENTILATOR_2;

    for (unsigned int i = block->count;i < 30;i++)
    {
        this->block->add(param);
        param.pos.y += 30.0f;
        param.pos.x += 30.0f;

        std::cout << "block.[" << i << "]texture_ID =" << block->texture[i] << "\n";
        std::cout << "block.[" << i << "]pos.x =" << block->pos[i].x << "\t";
        std::cout << "block.[" << i << "]pos.y =" << block->pos[i].y << "\n";
        std::cout << "block.[" << i << "]scale.x =" << block->scale[i].x << "\t";
        std::cout << "block.[" << i << "]scale.y =" << block->scale[i].y << "\n";

        param.tex = param.tex ^ (texture.VENTILATOR ^ texture.VENTILATOR_2);
    }
   
    

    // ПОДГОТОВКА ЗНАЧЕНИЙ ДЛЯ GPU 
    Batch();
    
    //;-------------------------------------------;//
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
    
    //ВЫЧИСЛЫНИЕ И ЗАПОЛНЕНИЕ БУФЕРА ЧТОБ ГПУ РИСОВАЛ ПОТОМ
    static const unsigned int X_indx = 0; 
    static const unsigned int Y_indx = 1; 
    static const unsigned int U_indx = 2;
    static const unsigned int V_indx = 3; 
    static const unsigned int ID_indx = 4;
    static const unsigned int STRIDE_F = 5; 

    for (unsigned int i = 0; i < block->count; i++)//каждая вершина  это 5 float (x y u v ,ID)
    {    unsigned int offset = i * 20;             //суммарное смещение буфера (в нем pos scale rotate texture и тд)
        
     float pos_X      = this->block->pos[i].x;
     float pos_Y      = this->block->pos[i].y;
     float scale_X    = this->block->scale[i].x;
     float scale_Y    = this->block->scale[i].y;
     float rotate_A   = this->block->rotate[i].x;
     unsigned int texture_ID = this->block->texture[i];
     
        for (unsigned int v = 0; v < 4; v++)
        {
            unsigned int v_off_idx = offset + (v * STRIDE_F);
            unsigned int v_off = v * 5;

            float local_x = this->render->vertex.square_tex[v_off + 0] * scale_X;
            float local_y = this->render->vertex.square_tex[v_off + 1] * scale_Y;
            // pos x y
            this->batch_ptr[v_off_idx + X_indx] = local_x + pos_X;
            this->batch_ptr[v_off_idx + Y_indx] = local_y + pos_Y;
            // tex u v
            this->batch_ptr[v_off_idx + U_indx] = this->render->vertex.square_tex[v_off + 3];
            this->batch_ptr[v_off_idx + V_indx] = this->render->vertex.square_tex[v_off + 4];

            this->batch_ptr[v_off_idx + ID_indx] = texture_ID;
        }
    }
}
void Engine::Tps()
{
    static const double TARGET_TPS = 100.0;
    static const double TICK_TIME = 1.0 / TARGET_TPS; // 0.01666... сек
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