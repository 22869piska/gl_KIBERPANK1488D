#include "GUI.hpp"
#include "..\engine.hpp"

void Gui::MenuManager()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	NATIVEPTR->gui_vertex->count = 0;

      if (NATIVEPTR->isMainMenu ==   true ) { MainMenuGui(); }
      if (NATIVEPTR->isEditorMode == true ) { EditorMenuGui(); }

}
//
void Gui::MainMenuGui()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	this->w = 200; this->h = 100;


	if (ADDButton(100.0f, 100 * 2, glm::vec3(0.0f, 0.5f, 0.0f), "PLAY")) //в гаме
	{
		NATIVEPTR->isMainMenu = false; NATIVEPTR->isEditorMode = false;
		NATIVEPTR->isGameMode = true;
		NATIVEPTR->LoadLevel();
		return;
	}
	if (ADDButton(100.0f, 100 * 4, glm::vec3(0.0f, 0.5f, 0.0f), "EDITOR")) //в редактор
	{
		NATIVEPTR->isMainMenu = false;
		NATIVEPTR->isEditorMode = true; NATIVEPTR->isGameMode = true;
		return;
	}
	if (ADDButton(100.0f, 100 * 6, glm::vec3(0.0f, 0.5f, 0.0f), "EXIT")) //exit
	{
		glfwSetWindowShouldClose(NATIVEPTR->window.window,1);
		return;
	}
		
}
void Gui::EditorMenuGui()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
    #define              PARAM EDITOR::spawninfo
	static const auto& BLOCK = Engine::native_ptr->block;
	static const auto& LIGHT = Engine::native_ptr->light;

	//ХУЙНЯ ЗАРАНЕЕ ->
	this->w = 150; this->h = 90;//Размер кнопок
	CheckMouseCollisionOnButton();
	glm::vec2 cursor_pos = this->ScreenToWorldOrtho(
		NATIVEPTR->mouse.x,NATIVEPTR->mouse.y,
		NATIVEPTR->window.width,NATIVEPTR->window.height,
	    NATIVEPTR->mvp.mvp);

	enum Type {
		//none = 0,
		block = 1,
		light = 2
	}; static Type type = block;

	static bool initialized = false;
	if (initialized == false)
	{
		PARAM.pos.x = INFINITY;
		PARAM.pos.y = INFINITY;

		BLOCK->count = 0;
		BLOCK->add(PARAM);
		LIGHT->count = 0;
		LIGHT->add(PARAM);

		NATIVEPTR->mouse.left_clicked_this_frame = false;
		initialized = true;
	}
	//Управление спавном ->
	
	//Swap type (TAB)
	if (glfwGetKey(NATIVEPTR->window.window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (NATIVEPTR->input_state.TAB_was_released)//bad
		{
			if (type == block) { BLOCK->pos_X[0] = INFINITY; BLOCK->pos_Y[0] = INFINITY; }
			if (type == light) { LIGHT->pos_X[0] = INFINITY; LIGHT->pos_Y[0] = INFINITY; }
			//
			int ntype = static_cast<int>(type) + 1;
			if (ntype > 2) { ntype = 1; }
			type = static_cast<Type>(ntype);
			//

			NATIVEPTR->input_state.TAB_was_released = false; 
		}
	}
	else { NATIVEPTR->input_state.TAB_was_released = true; }
	//Set param
	PARAM.pos = cursor_pos;

	//Метод спавна ->
		if (type == block)
		{
			BLOCK->pos_X[0] = cursor_pos.x;
			BLOCK->pos_Y[0] = cursor_pos.y;
			BLOCK->texture[0] = PARAM.tex;
		}
		if (type == light)
		{
			LIGHT->pos_X[0] = cursor_pos.x;
			LIGHT->pos_Y[0] = cursor_pos.y;
			LIGHT->radius[0] = PARAM.rad;
			LIGHT->intensity[0] = PARAM.intens;
			LIGHT->color_R[0] = PARAM.color.r;
			LIGHT->color_G[0] = PARAM.color.g;
			LIGHT->color_B[0] = PARAM.color.b;
			
		}
		
	if (NATIVEPTR->mouse.left_clicked_this_frame && this->mouse_over_gui == false)
	{
		if (type == block)
		{
			BLOCK->add(PARAM);
		}
		if (type == light)
		{
			LIGHT->add(PARAM);
		}
		//NATIVEPTR->Block_batch();
		//NATIVEPTR->Light_batch();
	}

}
//
bool Gui::ADDButton(float x, float y, const glm::vec3& color, const char* msg)
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	const float W = this->w, H = this->h;

	float mycolorR = color.r;
	float mycolorG = color.g;
	float mycolorB = color.b;
	const float stub_u = 0.1f;
	const float stub_v = 0.1f;

	float mouse_pos_x = NATIVEPTR->mouse.x * NATIVEPTR->window.scale_X;
	float mouse_pos_y = NATIVEPTR->mouse.y * NATIVEPTR->window.scale_Y;

	bool is_hovered = (mouse_pos_x >= x && mouse_pos_x <= (x + W) &&
		               mouse_pos_y >= y && mouse_pos_y <= (y + H));
	if (is_hovered == true) { mycolorR = color.g + 0.5f; }
	
	unsigned int idx = NATIVEPTR->gui_vertex->count;
	if (idx + 6 < MAX_GUI_VERTICES)
	{
		float x2 = x + W;
		float y2 = y + H;

		float vx[6] = {
			x,  x2, x,   
			x,  x2, x2  
		};
		float vy[6] = {
			y,  y,  y2, 
			y2, y,  y2   
		};
	
		for (int v = 0; v < 6; v++)
		{
			NATIVEPTR->gui_vertex->x[idx] = vx[v];
			NATIVEPTR->gui_vertex->y[idx] = vy[v];
			NATIVEPTR->gui_vertex->u[idx] = stub_u;
			NATIVEPTR->gui_vertex->v[idx] = stub_v;
			NATIVEPTR->gui_vertex->colorR[idx] = mycolorR;
			NATIVEPTR->gui_vertex->colorG[idx] = mycolorG;
			NATIVEPTR->gui_vertex->colorB[idx] = mycolorB;
			idx++;
		}
		NATIVEPTR->gui_vertex->count = idx;
	}

	if (msg != nullptr && msg[0] != '\0')
	{
		int text_len = 0;
		while (msg[text_len] != '\0') { text_len++; }

		float estimated_text_width = text_len * 16.0f;
		float display_h = 32.0f;

		// Точная формула центра [INDEX]
		float text_x = x + (W - estimated_text_width) / 2.0f;
		float text_y = y + (H - display_h) / 2.0f;

		ADDText(text_x, text_y, msg, glm::vec3(1.0f, 1.0f, 1.0f));
	}


	return is_hovered && NATIVEPTR->mouse.left_clicked_this_frame;
}
float Gui::ADDText(float x, float y, const char* text, glm::vec3 color)
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	const float W = this->w, H = this->h;

	const float char_w = 32.0f; //параметры шрифта
	const float char_h = 32.0f;
	const float tex_width = 512;//размер картинки
	const float tex_height = 192;
	const int columns = 16;

	// Размеры букв на экране
	const float display_w = 32.f;
	const float display_h = 32.f;

	unsigned int idx = NATIVEPTR->gui_vertex->count;
	float current_x = x;

	for (int i = 0; text[i] != '\0'; i++)
	{
		char c = text[i];

		int glyph_idx = static_cast<int>(c) - 32;
		if (glyph_idx < 0 || glyph_idx >= 96) continue;

		int col = glyph_idx % columns;
		int row = glyph_idx / columns;
		row = 5 - row; // Инверсия рядов для OpenGL

		float pixel_u1 = col * char_w;
		float pixel_v1 = row * char_h;
		float pixel_u2 = pixel_u1 + char_w;
		float pixel_v2 = pixel_v1 + char_h;

		//приведение к UV координатам текстуры 512х192 [INDEX]
		float u1 = pixel_u1 / tex_width;
		float v1 = pixel_v1 / tex_height;
		float u2 = pixel_u2 / tex_width;
		float v2 = pixel_v2 / tex_height;

		float x1 = current_x;
		float y1 = y;
		float x2 = current_x + display_w;
		float y2 = y + display_h;

		if (idx + 6 < MAX_GUI_VERTICES)
		{
			float vx[] = { x1, x2, x1,  x1, x2, x2 };
			float vy[] = { y1, y1, y2,  y2, y1, y2 };
			float vu[] = { u1, u2, u1,  u1, u2, u2 };
			float vv[] = { v2, v2, v1,  v1, v2, v1 };

			for (int v = 0; v < 6; v++)
			{
				NATIVEPTR->gui_vertex->x[idx] = vx[v];
				NATIVEPTR->gui_vertex->y[idx] = vy[v];
				NATIVEPTR->gui_vertex->u[idx] = vu[v];
				NATIVEPTR->gui_vertex->v[idx] = vv[v];
				NATIVEPTR->gui_vertex->colorR[idx] = color.r;
				NATIVEPTR->gui_vertex->colorG[idx] = color.g;
				NATIVEPTR->gui_vertex->colorB[idx] = color.b;
				idx++;
			}
		}

		current_x += display_w * 0.5f; // Шаг ровно в 16 пикселей [INDEX]
	}
	NATIVEPTR->gui_vertex->count = idx;
	return current_x - x;
}
glm::vec2 Gui::ScreenToWorldOrtho(float mx, float my, int windowWidth, int windowHeight, const glm::mat4& orthoMatrix)
{
	// Инвертируем Y, так как в GLFW (0,0)  это левый верхний угол, а в OpenGL  левый нижний
	float ndcX = (2.0f * mx) / (float)windowWidth - 1.0f;
	float ndcY = 1.0f - (2.0f * my) / (float)windowHeight;

	glm::mat4 inverseOrtho = glm::inverse(orthoMatrix);
	glm::vec4 worldPos = inverseOrtho * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);

	return glm::vec2(worldPos.x, worldPos.y);
}
void Gui::CheckMouseCollisionOnButton()
{
	this->mouse_over_gui = false;
	static const auto& NATIVEPTR = Engine::native_ptr;
	
	for (int i = 0;i < NATIVEPTR->gui_vertex->count;i++ )
	{
		if ((float)NATIVEPTR->mouse.x >= NATIVEPTR->gui_vertex->x[i] &&
			(float)NATIVEPTR->mouse.x <= NATIVEPTR->gui_vertex->x[i] + NATIVEPTR->gui_vertex->u[i] &&
			(float)NATIVEPTR->mouse.y >= NATIVEPTR->gui_vertex->y[i] &&
			(float)NATIVEPTR->mouse.y <= NATIVEPTR->gui_vertex->y[i] + NATIVEPTR->gui_vertex->v[i])
		{
			this->mouse_over_gui = true;
			break;
		}
	}	
}
