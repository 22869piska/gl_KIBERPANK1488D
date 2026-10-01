#include "GUI.hpp"
#include "..\engine.hpp"

void Gui::MenuManager()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	NATIVEPTR->gui_vertex->count = 0;
	this->mouse_over_gui = false;

      if (NATIVEPTR->isMainMenu ==   true ) { MainMenuGui(); }
      if (NATIVEPTR->isEditorMode == true ) { EditorMenuGui(); }

}
//
void Gui::MainMenuGui()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	this->w = 200; this->h = 100;


	if (ADDButton(100.0f, 100.0f * 2, glm::vec3(0.0f, 0.5f, 0.0f), "PLAY")) //в гаме
	{
		NATIVEPTR->isMainMenu = false; NATIVEPTR->isEditorMode = false;
		NATIVEPTR->isGameMode = true;
		NATIVEPTR->LoadLevel();
		return;
	}
	if (ADDButton(100.0f, 100.0f * 4, glm::vec3(0.0f, 0.5f, 0.0f), "EDITOR")) //в редактор
	{
		NATIVEPTR->isMainMenu = false;
		NATIVEPTR->isEditorMode = true; NATIVEPTR->isGameMode = true;
		return;
	}
	if (ADDButton(100.0f, 100.0f * 6, glm::vec3(0.0f, 0.5f, 0.0f), "EXIT")) //exit
	{
		glfwSetWindowShouldClose(NATIVEPTR->window.window,1);
		return;
	}
		
}
void Gui::EditorMenuGui()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	
	
	this->w = 100; this->h = 50;
	if (ADDButton(25.0f, 100.0f, glm::vec3(0.0f, 0.5f, 0.0f), "SAVE"))//
	{
		NATIVEPTR->SaveLevel();
	}
	if (ADDButton(25.0f, 25.0f, glm::vec3(0.0f, 0.5f, 0.0f), "LOAD"))//
	{
		NATIVEPTR->LoadLevel();
	}
	if (ADDButton(125.0f, 25.0f, glm::vec3(0.0f, 0.5f, 0.0f), "ASM"))//
	{
		NATIVEPTR->code_ptr;
	}

	
	NATIVEPTR->editor.EditorLogic();
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
	if (is_hovered == true) { mycolorR = color.g + 0.5f; this->mouse_over_gui = true; }

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
void Gui::ADDText(const float w, const float h, float x, float y, const char* text, glm::vec3 color)
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	const float W = w, H = h;

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
glm::vec2 Gui::ScreenToWorldOrtho()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	
	float ndcX = (2.0f * NATIVEPTR->mouse.x) / (float)NATIVEPTR->window.width - 1.0f;
	float ndcY = 1.0f - (2.0f * NATIVEPTR->mouse.y) / (float)NATIVEPTR->window.height;

	glm::mat4 inverseOrtho = glm::inverse(NATIVEPTR->mvp.mvp);
	glm::vec4 worldPos = inverseOrtho * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);

	return glm::vec2(worldPos.x, worldPos.y);
}

