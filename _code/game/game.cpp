#include"game\status.cpp"

namespace PIDORASGOVNOPENISDILDOSIXSEVEN1488
{
	//---------------------------------------------//
	void Run()
	{

		//glm::mat4 view = glm::mat4(1.f);
		attrib.view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 10.0f),  // позиция камеры
			glm::vec3(0.0f, 0.0f, 0.0f),  // цель камеры
			glm::vec3(0.0f, 1.0f, 0.0f)   // вектор "вверх"
		);
		//attrib.view = view;

	
		
		if (ultrastartmainmenu()) { BasicMenu(); }

	}


};