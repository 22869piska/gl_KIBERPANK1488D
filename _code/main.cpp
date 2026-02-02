#include "main.hpp"

int main()
{
	std::setlocale(LC_ALL, "Rus");


	InitGlContext();
	InitShaders();
	SpaceView();

	std::cout << "Инициализация прошла успешно" << std::endl;
	Tex.LoadAllTexture();


	

	std::cout << "Текстуры загружены" << std::endl;

	if (true) {};
	PIDORASGOVNOPENISDILDOSIXSEVEN1488::Run();


	std::cout << "КОНЕЦ ВАСЯ 67!" << std::endl;
	glfwTerminate();

	return 0;
}
