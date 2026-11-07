#include "engine.hpp"
#include "CALLBACK.hpp"
#include "../asm/asm.hpp"

#define RUNTIME_PROC ((JIT_ASM::LogicFunc)JIT_ASM::code_ptr)(this);
Engine* Engine::native_ptr = nullptr;
static bool PickFile(HWND owner, char* out, const char* filter, const char* initDirRel)
{
    char initDir[MAX_PATH] = {};
    GetFullPathNameA(initDirRel, MAX_PATH, initDir, nullptr);

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = owner;
    ofn.lpstrFile = out;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = initDir;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    return GetOpenFileNameA(&ofn) == TRUE;
}

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
    this->texture.texFont = this->texture.LoadTexture2d("_code/TEXTURES/font/DOS.png");
    
   
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(JIT_ASM::code_mem, &mbi, sizeof(mbi));
    printf("protect: 0x%lX  (0x40 = RWX, 0x04 = RW, 0x02 = R)\n", mbi.Protect);

    if (ASM::RP_SIZE > ASM_CODE_SIZE) { std::cout << "!!!!!!!!!!!!!!!!ASM_CODE > MEMORY_ALLOC_SIZE!\n"; return; }
    memcpy(JIT_ASM::code_ptr, ASM::RuntimeProc_START, ASM::RP_SIZE);//ASM КОД копироовать из .ASM В ПРОЦЕДУРУ РАНТАЙМА //ОБЯЗАТЕЛЬНО ВЕДЬ НУЖЕН RET В КОНЦE
    std::cout << "ASM OPCODES SIZE: " << ASM::RP_SIZE << " bytes \t [ MAX " << ASM_CODE_SIZE << " ] bytes\n";
    //printf("ADDR == THIS->BLOCK_BASE: %llu\n", (uintptr_t)&this->block_mem - (uintptr_t)this);
    //printf("ADDR == THIS->BLOCK_LIGHT: %llu\n", (uintptr_t)&this->light_mem - (uintptr_t)this);
    //printf("ADDR == THIS->ASM_MY_MEMORY: %llu\n", (uintptr_t)&this->MY_MEM - (uintptr_t)this);
   
    Callback::FramebufferSize(this->window.window, this->window.width, this->window.height);
}
void Engine::Run()
{
    while (!glfwWindowShouldClose(window.window))
    {
        this->input_state.InputClear();
        glfwPollEvents();
        CPU();

        this->render->GPU();
        Tps();
        glfwSwapBuffers(window.window);
    }
}
//-------------------------------------------//
void Engine::CPU()
{
    gui.MenuManager();
   
    //;-------------------------------------------;//
    unsigned long long clock = ASM::ClockCounter();
    //;-------------------------------------------;//
    if (this->isGameMode == true)
    {
        RUNTIME_PROC
    }
    //;-------------------------------------------;//
    Batch();// ПОДГОТОВКА ЗНАЧЕНИЙ ДЛЯ GPU 
    clock = ASM::ClockCounter() - clock;
    //;-------------------------------------------;//
    //std::cout <<" clock_per_frame_CPU -> " << clock << " \n";
}
//-------------------------------------------//
//private ->
void Engine::SpaceView()
{
    const float PARAM1 = (-VIRTUAL_WIDTH / 2.0f) * camera_zoom;
    const float PARAM2 = ( VIRTUAL_WIDTH / 2.0f) * camera_zoom;
    const float PARAM3 = (-VIRTUAL_HEIGHT / 2.0f) * camera_zoom;
    const float PARAM4 = ( VIRTUAL_HEIGHT / 2.0f) * camera_zoom;
  
    mvp.projection = glm::ortho(
        PARAM1, PARAM2,
        PARAM3, PARAM4,
        -100.0f, 100.0f
    );
    mvp.view = glm::translate(glm::mat4(1.0f), glm::vec3(-this->camera_posX, -this->camera_posY, 0.0f));
    mvp.mvp = mvp.projection * mvp.view;
    //->
    mvp.gui_ortho = glm::ortho(0.0f, (float)VIRTUAL_WIDTH, (float)VIRTUAL_HEIGHT, 0.0f, -1.0f, 1.0f);

}
void Engine::Batch()
{
    SpaceView();
    Block_batch();
    Light_batch();
  
}
void Engine::Block_batch()
{
    const unsigned int count = this->block->count;
    if (count == 0) return;

    float* const __restrict dst = this->batch_block_ptr;
    const float* const src_tex = this->render->vertex.square_tex;

    const float* const __restrict b_x = this->block->pos_X;
    const float* const __restrict b_y = this->block->pos_Y;
    const float* const __restrict b_sx = this->block->scale_X;
    const float* const __restrict b_sy = this->block->scale_Y;
    const unsigned int* const __restrict b_tex = this->block->texture;

    const float u0 = src_tex[3];  const float v0 = src_tex[4];
    const float u1 = src_tex[8];  const float v1 = src_tex[9];
    const float u2 = src_tex[13]; const float v2 = src_tex[14];
    const float u3 = src_tex[18]; const float v3 = src_tex[19];

    const float l_x0 = src_tex[0];  const float l_y0 = src_tex[1];
    const float l_x1 = src_tex[5];  const float l_y1 = src_tex[6];
    const float l_x2 = src_tex[10]; const float l_y2 = src_tex[11];
    const float l_x3 = src_tex[15]; const float l_y3 = src_tex[16];

    for (unsigned int i = 0; i < count; i++)
    {
        // Вычисляем линейный индекс в выходном буфере (GPU VBO)
        const unsigned int offset = i * 20;

        // Загружаем свойства текущего блока в регистры
        const float pos_X = b_x[i];
        const float pos_Y = b_y[i];
        const float scale_X = b_sx[i];
        const float scale_Y = b_sy[i];
        const float texture_ID = static_cast<float>(b_tex[i]); // Приводим к float один раз

        // ВЕРШИНА 0
        dst[offset + 0] = l_x0 * scale_X + pos_X;
        dst[offset + 1] = l_y0 * scale_Y + pos_Y;
        dst[offset + 2] = u0;
        dst[offset + 3] = v0;
        dst[offset + 4] = texture_ID;

        // ВЕРШИНА 1
        dst[offset + 5] = l_x1 * scale_X + pos_X;
        dst[offset + 6] = l_y1 * scale_Y + pos_Y;
        dst[offset + 7] = u1;
        dst[offset + 8] = v1;
        dst[offset + 9] = texture_ID;

        // ВЕРШИНА 2
        dst[offset + 10] = l_x2 * scale_X + pos_X;
        dst[offset + 11] = l_y2 * scale_Y + pos_Y;
        dst[offset + 12] = u2;
        dst[offset + 13] = v2;
        dst[offset + 14] = texture_ID;

        // ВЕРШИНА 3
        dst[offset + 15] = l_x3 * scale_X + pos_X;
        dst[offset + 16] = l_y3 * scale_Y + pos_Y;
        dst[offset + 17] = u3;
        dst[offset + 18] = v3;
        dst[offset + 19] = texture_ID;
    }
  /*  //ВЫЧИСЛЫНИЕ И ЗАПОЛНЕНИЕ БУФЕРА ЧТОБ ГПУ РИСОВАЛ ПОТОМ
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
    }*/
}
void Engine::Light_batch()
{
    float* b_ptr = this->batch_light_ptr; // Указатель на массив для GPU

    for (unsigned int i = 0; i < light->count; i++) {
        
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
        if (isGameMode == true && isEditorMode == false) { std::cout<< "TPS -> " << ticks << "\n"; }
        ticks = 0;
        log_timer = glfwGetTime();
    }
}
void Engine::SaveLevel()
{
    char szFile[260] = { 0 };
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Если есть хендл окна GLFW, можно вытащить через glfwGetWin32Window
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    ofn.lpstrFilter = 0;//переделать потом код тут а то впадлу было ебать
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;

    ofn.lpstrInitialDir = "temp"; // Стартовая папка, которая откроется по умолчанию 
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        const char* path = szFile;
    
        // Все OBJ
        BIN_IO::Save(this->block, sizeof(Block), path); // Записали блок (создали файл)
        BIN_IO::SaveAppend(this->light, sizeof(Light), path); // Дописали свет в конец

        //CODE  asm
        BIN_IO::SaveAppend(this->MY_MEM, sizeof(this->MY_MEM), path); //MY_MEM
        BIN_IO::SaveAppend(JIT_ASM::code_ptr, ASM_CODE_SIZE, path); //Дaльше code дописываем
    }
}
bool Engine::SaveCodeInLevelFileFromFile()
{
    HWND owner = 0;

    //какой .bin вписываем
    char binPath[MAX_PATH] = {};
    if (!PickFile(owner, binPath, "Binary code (*.bin)\0*.bin\0", "temp/code/bin_code")) return false;

    FILE* f = nullptr;
    if (fopen_s(&f, binPath, "rb") != 0 || !f) { std::cout << "cannot open: " << binPath << "\n"; return false; }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size <= 0 || size > ASM_CODE_SIZE) {
        std::cout << "bad bin size: " << size << " (max " << ASM_CODE_SIZE << ")\n";
        fclose(f);
        return false;
    }
    std::vector<unsigned char> code(ASM_CODE_SIZE, 0xCC);   // хвост = int3
    size_t got = fread(code.data(), 1, size, f);
    fclose(f);
    if (got != (size_t)size) { std::cout << "read error\n"; return false; }

    //какой уровень вписываем
    char lvlPath[MAX_PATH] = {};
    if (!PickFile(owner, lvlPath, "Level (*.*)\0*.*\0", "temp/level")) return false;

    //позиция секции кода внутри файла уровня
    const long long CODE_OFFSET = (long long)sizeof(Block) + sizeof(Light) + sizeof(this->MY_MEM);

    FILE* lv = nullptr;
    if (fopen_s(&lv, lvlPath, "r+b") != 0 || !lv) { std::cout << "cannot open level\n"; return false; }  // r+b: не обрезает файл

    _fseeki64(lv, 0, SEEK_END);
    long long lvSize = _ftelli64(lv);
    if (lvSize < CODE_OFFSET) {
        std::cout << "level file too short (" << lvSize << " < " << CODE_OFFSET << "), wrong file or old format\n";
        fclose(lv);
        return false;
    }

    _fseeki64(lv, CODE_OFFSET, SEEK_SET);
    size_t wr = fwrite(code.data(), 1, ASM_CODE_SIZE, lv);
    fclose(lv);
    if (wr != (size_t)ASM_CODE_SIZE) { std::cout << "write error\n"; return false; }

    std::cout << "code " << size << " bytes -> " << lvlPath << " @0x" << std::hex << CODE_OFFSET << std::dec << "\n";
    return true;
}
void Engine::LoadCodeInMemoryFromFile()
{
    char szFile[MAX_PATH] = { 0 };
    char initDir[MAX_PATH] = { 0 };

    GetFullPathNameA("temp/code/bin_code", MAX_PATH, initDir, nullptr);

    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = 0;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Binary code (*.bin)\0*.bin\0All files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrInitialDir = initDir;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) != TRUE)
        return;

    FILE* f = nullptr;
    errno_t err = fopen_s(&f, szFile, "rb");
    if (err != 0 || !f) {
        std::cout << "cannot open: " << szFile << " (Error code: " << err << ")\n";
        return;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0 || size > ASM_CODE_SIZE) {
        std::cout << "bad size: " << size << " (max " << ASM_CODE_SIZE << ")\n";
        fclose(f);
        return;
    }

    std::vector<unsigned char> tmp(size);
    size_t got = fread(tmp.data(), 1, size, f);
    fclose(f);

    if (got != (size_t)size) {
        std::cout << "read error\n";
        return;
    }

    // Запись в буфер: остаток заполняем int3 
    memset(JIT_ASM::code_ptr, 0xCC, ASM_CODE_SIZE);
    memcpy(JIT_ASM::code_ptr, tmp.data(), size);

    FlushInstructionCache(GetCurrentProcess(), JIT_ASM::code_ptr, ASM_CODE_SIZE);

    std::cout << "loaded " << size << " bytes from " << szFile << "\n";
}
bool Engine::LoadLevel()
{
    char szFile[260] = { 0 };
    OPENFILENAMEA ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL; // Если есть хендл окна GLFW, можно вытащить через glfwGetWin32Window
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);

    ofn.lpstrFilter = "";// Хуйня
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;

    ofn.lpstrInitialDir = "temp"; // Стартовая папка, которая откроется по умолчанию 
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn) == TRUE)
    {
        std::cout << "load memory from hard disk: " << szFile << "\n";

        const char* path = szFile;
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
        BIN_IO::Load(JIT_ASM::code_ptr, ASM_CODE_SIZE, path, offset);

        for (int i = 0; i < sizeof(this->MY_MEM); i++)
        {
            std::cout << std::bitset<8>(MY_MEM[i]); std::cout << "|>>|";
            if ((i + 1) % 4 == 0) { std::cout << '\n'; }

        }
        return true;
    }
    else { return false; }
        //{ this->isGameMode = false; this->isEditorMode = false; this->isMainMenu = true;}
}
void Engine::GetAllAddrObjOffset()
{
    const uintptr_t OFFSET_THIS_PTR = (uintptr_t)this;

    //block->
    const uintptr_t block_offset_posX = (uintptr_t)block->pos_X - OFFSET_THIS_PTR;
    const uintptr_t block_offset_posY = (uintptr_t)block->pos_Y - OFFSET_THIS_PTR;
    const uintptr_t block_offset_scaleX = (uintptr_t)block->scale_X - OFFSET_THIS_PTR;
    const uintptr_t block_offset_scaleY = (uintptr_t)block->scale_Y - OFFSET_THIS_PTR;
    const uintptr_t block_offset_rotateA = (uintptr_t)block->rotate_A - OFFSET_THIS_PTR;
    const uintptr_t block_offset_texture = (uintptr_t)block->texture - OFFSET_THIS_PTR;
    const uintptr_t block_offset_flag = (uintptr_t)block->flag - OFFSET_THIS_PTR;

    std::printf("==============================BLOCK OFFSET==============================\n");
    std::printf("ptr this->            OFFSET -> HEX: +0x%zX   or  DEC: %llu bytes\n", OFFSET_THIS_PTR, (unsigned long long)OFFSET_THIS_PTR);

    std::printf("float BLOCK->pos_X    OFFSET -> HEX: +0x%zX   or  DEC: %llu bytes\n", block_offset_posX, (unsigned long long)block_offset_posX);
    std::printf("float BLOCK->pos_Y    OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", block_offset_posY, (unsigned long long)block_offset_posY);
    std::printf("float BLOCK->scale_X  OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", block_offset_scaleX, (unsigned long long)block_offset_scaleX);
    std::printf("float BLOCK->scale_Y  OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", block_offset_scaleY, (unsigned long long)block_offset_scaleY);
    std::printf("float BLOCK->rotate_A OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", block_offset_rotateA, (unsigned long long)block_offset_rotateA);
    std::printf("float BLOCK->texture  OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", block_offset_texture, (unsigned long long)block_offset_texture);
    std::printf("uint BLOCK->flag      OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", block_offset_flag, (unsigned long long)block_offset_flag);

    //light->
    const uintptr_t light_offset_posX = (uintptr_t)light->pos_X - OFFSET_THIS_PTR;
    const uintptr_t light_offset_posY = (uintptr_t)light->pos_Y - OFFSET_THIS_PTR;
    const uintptr_t light_offset_colorR = (uintptr_t)light->color_R - OFFSET_THIS_PTR;
    const uintptr_t light_offset_colorG = (uintptr_t)light->color_G - OFFSET_THIS_PTR;
    const uintptr_t light_offset_colorB = (uintptr_t)light->color_B - OFFSET_THIS_PTR;
    const uintptr_t light_offset_radius = (uintptr_t)light->radius - OFFSET_THIS_PTR;
    const uintptr_t light_offset_intensity = (uintptr_t)light->intensity - OFFSET_THIS_PTR;

    std::printf("==============================LIGHT OFFSET==============================\n");

    std::printf("float LIGHT->pos_X     OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", light_offset_posX, (unsigned long long)light_offset_posX);
    std::printf("float LIGHT->pos_Y     OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", light_offset_posY, (unsigned long long)light_offset_posY);
    std::printf("float LIGHT->color_R   OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", light_offset_colorR, (unsigned long long)light_offset_colorR);
    std::printf("float LIGHT->color_G   OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", light_offset_colorG, (unsigned long long)light_offset_colorG);
    std::printf("float LIGHT->color_B   OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", light_offset_colorB, (unsigned long long)light_offset_colorB);
    std::printf("float LIGHT->radius    OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", light_offset_radius, (unsigned long long)light_offset_radius);
    std::printf("float LIGHT->intensity OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", light_offset_intensity, (unsigned long long)light_offset_intensity);

    //memofst->
    const uintptr_t asm_mem_offset = (uintptr_t)this->MY_MEM - OFFSET_THIS_PTR;
    const uintptr_t camera_pos_x_offset = (uintptr_t)&this->camera_posX - OFFSET_THIS_PTR;
    const uintptr_t camera_pos_y_offset = (uintptr_t)&this->camera_posY - OFFSET_THIS_PTR;

    const uintptr_t key_start_addr = (uintptr_t)&this->input_state.keys_alpha[0] - OFFSET_THIS_PTR;
    const uintptr_t key_end_addr   = (uintptr_t)&this->input_state.TAB_was_released - OFFSET_THIS_PTR;


    std::printf("==============================MEM OFFSET==============================\n");

    std::printf("char  MY_MEM[]    OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n",asm_mem_offset, (unsigned long long)asm_mem_offset);
    std::printf("float camera_posX OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n",camera_pos_x_offset, (unsigned long long)camera_pos_x_offset);
    std::printf("float camera_posY OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n",camera_pos_y_offset, (unsigned long long)camera_pos_y_offset);

    std::printf("bool key_start_addr OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", key_start_addr, (unsigned long long) key_start_addr);
    std::printf("bool key_end_addr OFFSET ->   HEX: +0x%zX  or  DEC: %llu bytes\n"  , key_end_addr, (unsigned long long) key_end_addr);

    for (unsigned int i = 0;i < 26;i++)
    {
        std::printf("%u", i);
        std::printf(" KEY OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", key_start_addr + i, key_start_addr + i);
    }

    //code_ptr->
    const uintptr_t code_ptr_offset = (uintptr_t)JIT_ASM::code_ptr - OFFSET_THIS_PTR;
    uintptr_t base = (uintptr_t)GetModuleHandle(nullptr);

    std::printf("==============================OPCODES OFFSET==============================\n");

    std::printf("ptr ASM OPCODES OFFSET -> HEX: +0x%zX  or  DEC: %llu bytes\n", code_ptr_offset, (unsigned long long) code_ptr_offset);
    printf("ptr code_mem RVA: 0x%llX\n", (unsigned long long)((uintptr_t)JIT_ASM::code_mem - base));

    std::cout << "ASM OPCODES SIZE: " << ASM::RP_SIZE << " bytes \t [ MAX " << ASM_CODE_SIZE << " ] bytes\n";
    if (ASM::RP_SIZE > ASM_CODE_SIZE) { std::cout << "!!!!!!!!!!!!!!!!ASM_CODE > MEMORY_ALLOC_SIZE!\n"; return; }

}
//-------------------------------------------//
//init ->
//-------------------------------------------//
void Engine::macro_def()
{
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
    glfwSetMouseButtonCallback(this->window.window, Callback::MouseButtonCallback);
    glfwSetScrollCallback(this->window.window, Callback::ScrollCallback);
}
//-------------------------------------------//
