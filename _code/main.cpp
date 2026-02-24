#include "main.hpp"
//#pragma comment(lib, "winmm.lib")

int main()
{
	std::setlocale(LC_ALL, "Rus");
	timeBeginPeriod(1);

	InitGlContext();
	InitShaders();
	SpaceView();

	
	//glfwSwapInterval(1);

	std::cout << "Инициализация прошла успешно" << std::endl;
	Tex.LoadAllTexture();

	std::cout << Tex.texture_count <<" >> Текстуры загружены" << std::endl;
	//----/

	gl_cd.view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 10.0f),  // позиция камеры
		glm::vec3(0.0f, 0.0f, 0.0f),  // цель камеры
		glm::vec3(0.0f, 1.0f, 0.0f)   // вектор "вверх"
	);

	venom.attrib.set_color = glm::vec3(1,0,1);
	venom.attrib.set_pos = glm::vec2(0,100);
	venom.attrib.set_rotate = glm::vec2(0,0);
	venom.attrib.set_scale = glm::vec2(350,340);
	venom.attrib.texture_ = Tex.vjlink_granate;

	venom.add();

	glfwSetTime(0);
	//----/

	while (!glfwWindowShouldClose(gl_cd.window))
	{
		
		//C_TIME = glfwGetTime();
		glClearColor(0.1, 0.f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		for (int i = 0;i < venom.obj_count;i++)
		{
			DrawSprite(venom.objects[i]);
			std::cout << "venom.ID " << venom.objects[i].ID << " <<| \n";
		}
		if (glfwGetKey(gl_cd.window,GLFW_KEY_W) == GLFW_PRESS )
		{
			venom.attrib.set_pos = glm::vec2( (int)glfwGetTime() * rand()%800, (int)glfwGetTime() * rand() % 800 );
			venom.add();

		}


		Update();
	}
	//----/
	std::cout << "КОНЕЦ ВАСЯ 67!" << std::endl;
	glfwTerminate();
	timeEndPeriod(1);
	return 0;
}
