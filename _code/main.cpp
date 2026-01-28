#include "main.hpp"

int main()
{

	InitGlContext();
	InitShaders();
	SpaceView();


	while (!glfwWindowShouldClose(gl_cd.window))
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//DrawSprite();
		glm::mat4 view = glm::mat4(1.0f);

		view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 3.0f),  // позиция камеры
			glm::vec3(0.0f, 0.0f, 0.0f),  // цель камеры
			glm::vec3(0.0f, 1.0f, 0.0f)   // вектор "вверх"
		);
		attrib.view = view;
		//gl_cd.shader_id.test_shader->setMat4("view", view);
		
		glm::vec3 arr[5] = {
			glm::vec3(0, 0, 0),	
			glm::vec3(-100, -100, 0),
			glm::vec3(-100, 100, 0),  
			glm::vec3(100, -100, 0),
			glm::vec3(100, 100, 0)
		};
		
		
		for (int i = 0; i < 5; i++)
		{
			
			attrib.set_pos = arr[i];
			

			attrib.set_scale = glm::vec2(25, 25);
			attrib.set_scale += glm::vec2(25*i, 25*i);
			
			//PutPixel();
			DrawSprite();
		}
			


		glfwSwapBuffers(gl_cd.window);
		glfwPollEvents();
	}

	glfwTerminate();

	//END//
	std::cout << "hello world \n";
	return 0;
}