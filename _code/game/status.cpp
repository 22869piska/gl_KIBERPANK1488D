//namespace menu
//{	
#ifdef __INTELLISENSE__
      


#endif


	bool ultrastartmainmenu()
	{

			std::cout << "START";

            float scale_button1 = 2.0f;
            float scale_button2 = 1.0f;

            
            bool menu_active = true;
            bool chose_exit = 0;

            while (menu_active && !glfwWindowShouldClose(gl_cd.window))
            {
           
                
                if (glfwGetKey(gl_cd.window, GLFW_KEY_DOWN) == GLFW_PRESS)
                {
                    scale_button1 = 1.0f;
                    scale_button2 = 2.0f;
                    chose_exit = 1;
                    std::cout << "BOOM";

                }
                else if (glfwGetKey(gl_cd.window, GLFW_KEY_UP) == GLFW_PRESS)
                {
                    scale_button1 = 2.0f;
                    scale_button2 = 1.0f;

                    chose_exit = 0;
                
                }
                else if (glfwGetKey(gl_cd.window, GLFW_KEY_ENTER) == GLFW_PRESS)
                {
                  
                    if (chose_exit == 1)
                    {
                        menu_active = false;

                        continue;
                    }
                   
                }
              
              
                glClearColor(scale_button2,0.f , scale_button1, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);


                attrib.set_pos = glm::vec2(0, 200);
                attrib.set_scale = glm::vec2(500, 150 * scale_button1);
                attrib.texture_ = Tex.XAMAC_GUN;
                DrawSprite();

               
                attrib.set_pos = glm::vec2(0, -200);
                attrib.set_scale = glm::vec2(500, 150 * scale_button2);
                attrib.texture_ = Tex.vjlink_granate;
                DrawSprite();

            //---//
               
                SpaceView();

                glfwSwapBuffers(gl_cd.window);
                glfwPollEvents();
            }
            
            return chose_exit;
         
		
	}
	void BasicMenu()
	{

			std::cout << "Basic";

            double buttons[5] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
            int current_button = 0;
            
            int max)_
            bool menu_active = true;
            bool chose_exit = 0;

            while (menu_active && !glfwWindowShouldClose(gl_cd.window))
            {
           
                
                if (glfwGetKey(gl_cd.window, GLFW_KEY_DOWN) == GLFW_PRESS)
                {
                    if (current_button == 0) {
                        current_button = 4;
                        buttons[0] =
                            buttons[4] == 2.0f;
                    }

                }
                else if (glfwGetKey(gl_cd.window, GLFW_KEY_UP) == GLFW_PRESS)
                {
                    if (current_button == 4) {
                        current_button = 0;
                        buttons[current_button] == 2.0f;
                    }
                }
                else if (glfwGetKey(gl_cd.window, GLFW_KEY_ENTER) == GLFW_PRESS)
                {
                  
                    if (chose_exit == 1)
                    {
                        menu_active = false;
                        continue;
                    }
                   
                }
              
              
                glClearColor(scale_button2,0.f , scale_button1, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);


                attrib.set_pos = glm::vec2(0, 200);
                attrib.set_scale = glm::vec2(500, 150 * scale_button1);
                attrib.texture_ = Tex.XAMAC_GUN;
                DrawSprite();

               
                attrib.set_pos = glm::vec2(0, -200);
                attrib.set_scale = glm::vec2(500, 150 * scale_button2);
                attrib.texture_ = Tex.vjlink_granate;
                DrawSprite();

            //---//
               
                SpaceView();

                glfwSwapBuffers(gl_cd.window);
                glfwPollEvents();
            }
		
	}
//};
