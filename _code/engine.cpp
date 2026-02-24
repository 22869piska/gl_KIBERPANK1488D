#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <iostream>
//#include <>
#include <filesystem>

#include <cmath>
#include <thread>   
#include <chrono> 

#include"SHADER.hpp"
#include"textures.cpp"


//------------------//
//      struct      //
//------------------//
int width = 1200;
int height = 840;
Textures Tex;

int KEY_STATE = 0;
double C_TIME;// = (float)glfwGetTime();

//-----//
struct Vertices
{
public:

    float triangle[12] = {
        // positions          // colors
         0.5f,  0.5f, 0.0f,    // top right
         0.5f, -0.5f, 0.0f,    // bottom right
        -0.5f, -0.5f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   
    };
    unsigned int indices[6] = {
       0, 1, 3, // first triangle
       1, 2, 3  // second triangle
    };
    //square_tex->
    float square_tex[20] = {
        // positionsy// texture coords
         0.5f,  0.5f, 0.0f,    1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,    1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,    0.0f, 1.0f
    };
};
struct ShaderIDs {

    //ультра тест треугольника
    Shader* test_shader;
    GLuint VAO_triangle, VBO_triangle;   
    GLuint EBO;
    //texure shader спрайты ->
    Shader* tex_shader;
    GLuint tex_VAO; GLuint tex_VBO;
    GLuint tex_EBO;
};

struct GlContextData
{
 public:
    GLFWwindow* window = 0;
    Vertices v; ShaderIDs shader_id;
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
}; GlContextData gl_cd;

//------------------//
// struct Attribute //
//------------------//

struct Attribute
{
public:
    glm::vec2 set_pos =    glm::vec2(0.f, 0.f);
    glm::vec2 set_scale =  glm::vec2(0.f, 0.f);
    glm::vec2 set_rotate = glm::vec2(0.f, 0.f);
    glm::vec3 set_color =  glm::vec3(0.f, 0.f, 0.f);
    // Texture* tex = 0;
    unsigned int texture_ = 0;    //40 

   // glm::mat4 view = (1.f);

}; Attribute attrib;

//------------------//
//       proc       //
//------------------//

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
/*
void physics_player(float acceleration_x, float acceleration_y, float velocity_x, float velocity_y){
    float maxSpeed = 50.0f; //добавить в глобальные переменные
    float friction = 0.7f;  //добавить в глобальные переменные
    float lastFrame = 0.0f; //добавить в глобальные переменные
    float velocity;         //добавить в глобальные переменные
    float moving_x, moving_y;       //добавить в глобальные переменные //слышно меня вася? венам сиксеве();
    float deltaTime = currentFrame - lastFrame;
    float lastFrame = currentFrame;
    velocity_x += acceleration_x * deltaTime;
    velocity_y += acceleration_y * deltaTime;
    velocity = sqrt(std::pow(velocity_x, 2) + std::pow(velocity_x, 2)) * friction;//добавить в рантайм
    moving_x = velocity_x * deltaTime;
    moving_y = velocity_y * deltaTime;
}
*/
//-->
void InitGlContext()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    gl_cd.window = glfwCreateWindow(width,height, "openASSglfw", NULL, NULL);
    if (gl_cd.window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(gl_cd.window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glfwSetFramebufferSizeCallback(gl_cd.window, framebuffer_size_callback);
    //glfwSetKeyCallback(gl_cd.window, key_callback);
}
void InitShaders()
{
    //3ygolniki->
    glGenVertexArrays(1, &gl_cd.shader_id.VAO_triangle);
    glGenBuffers(1, &gl_cd.shader_id.VBO_triangle);
    glGenBuffers(1, &gl_cd.shader_id.EBO);
    //
    glBindVertexArray(gl_cd.shader_id.VAO_triangle);
    glBindBuffer(GL_ARRAY_BUFFER, gl_cd.shader_id.VBO_triangle);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_cd.v.triangle), gl_cd.v.triangle, GL_STATIC_DRAW);
    //-kubiki->
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_cd.shader_id.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gl_cd.v.indices), gl_cd.v.indices, GL_STATIC_DRAW);
    //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //--//
    gl_cd.shader_id.test_shader = new Shader("_code/shaders/VERTEX_SHADER.shader", "_code/shaders/FRAGMENT_SHADER.shader");
    
    //next shader->
    //---------------------------------//
    //---------------------------------//
    glGenVertexArrays(2, &gl_cd.shader_id.tex_VAO);
    glGenBuffers(2, &gl_cd.shader_id.tex_VBO);
    glGenBuffers(2, &gl_cd.shader_id.EBO);
    //
    glBindVertexArray(gl_cd.shader_id.tex_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gl_cd.shader_id.tex_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(gl_cd.v.square_tex), gl_cd.v.square_tex, GL_STATIC_DRAW);
    //-kubiki->
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_cd.shader_id.tex_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gl_cd.v.indices), gl_cd.v.indices, GL_STATIC_DRAW);
    //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //--//
    gl_cd.shader_id.tex_shader = new Shader("_code/shaders/TexSHADER/VERT_SHADER.shader", "_code/shaders/TexSHADER/FRAG_SHADER.shader");
    //-----------------------------------------------------------------//
    return;
}

void SpaceView()
{
    gl_cd.projection = glm::ortho(-1.0f * width / 2, 1.0f * width / 2, -1.0f * height / 2, 1.0f * height / 2, 0.1f, 100.0f);
   
    gl_cd.shader_id.test_shader->use();
    gl_cd.shader_id.test_shader->setMat4("projection", gl_cd.projection);
    gl_cd.shader_id.test_shader->setMat4("view", gl_cd.view);

    gl_cd.shader_id.tex_shader->use();
    gl_cd.shader_id.tex_shader->setMat4("projection", gl_cd.projection);
    gl_cd.shader_id.tex_shader->setMat4("view", gl_cd.view);
}
//----------------------------//
void __fastcall PutPixel()
{
    glUseProgram(gl_cd.shader_id.test_shader->ID);
    glBindVertexArray(gl_cd.shader_id.EBO);

    glm::mat4 model = glm::mat4(1.0f);
    
    model = glm::translate(model,glm::vec3(attrib.set_pos.x, attrib.set_pos.y,0.f));
    model = glm::scale(model,glm::vec3(attrib.set_scale.x, attrib.set_scale.y,0.f));

    gl_cd.shader_id.test_shader->setMat4("model", model);
    gl_cd.shader_id.test_shader->setVec3("color", attrib.set_color);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}
void __fastcall DrawSprite(const VENOM::OBJ& obj)
{
    glUseProgram(gl_cd.shader_id.tex_shader->ID);
    glBindVertexArray(gl_cd.shader_id.tex_EBO);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(obj.set_pos.x, obj.set_pos.y, 0.f));
    model = glm::scale(model, glm::vec3(obj.set_scale.x, obj.set_scale.y, 0.f));

    gl_cd.shader_id.tex_shader->setMat4("model", model);
    gl_cd.shader_id.tex_shader->setMat4("view", gl_cd.view);

    glBindTexture(GL_TEXTURE_2D, obj.texture_); // надо переместить ету команду нахуй. неоптимизировано, слишком медленно даже в простое ето нагруза на GPU
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    //->
    

}
 double skok_t = 0.0;
 unsigned int frames = 0;

void Tps()
{
   
    const double MS = 0.01f;//ms

    double start_time = C_TIME;
    double current_time = glfwGetTime();

    double time = current_time - start_time;
    double sleep_time = MS - time;


    int micro_seconds = (int)(sleep_time * 1000000.0);
    std::this_thread::sleep_for(std::chrono::microseconds((micro_seconds - 1000)));
    while (glfwGetTime() - start_time < MS) {}
   
    double full_frame_time = glfwGetTime() - start_time;
    if (skok_t < 1.0) { skok_t += full_frame_time; frames++; }
    else { std::cout << frames << "FRAMES \n"; skok_t = 0; frames = 0; }

    //std::cout << time       << "<<|time|\n";
    //std::cout << sleep_time << "<<|sleep_time|\n";
    C_TIME = glfwGetTime(); 
    
}

//------//
void Update()
{

    //
    SpaceView();
    glfwSwapBuffers(gl_cd.window);
    glfwPollEvents();
    Tps();
   
}
//------------------//
 

 
