#include "GUI.hpp"
#include "..\engine.hpp"

void Gui::MenuManager()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	NATIVEPTR->gui_vertex->count = 0;
	
	if (NATIVEPTR->isMainMenu == true) { MainMenuGui(); }
	

	NATIVEPTR->mouse.left_clicked_this_frame = false;
}
//
void Gui::MainMenuGui()
{
	static const auto& NATIVEPTR = Engine::native_ptr;

	if (ADDButton(100.0f, 100 * 2, glm::vec3(0.0f, 0.5f, 0.0f)))
	{
		NATIVEPTR->isMainMenu = false; NATIVEPTR->isGameMode = true;
		NATIVEPTR->LoadLevel();
		std::cout << "button_pushed";

		return;
	}
	if (ADDButton(100.0f, 100 * 4, glm::vec3(0.0f, 0.5f, 0.0f)))
	{
		glfwSetWindowShouldClose(NATIVEPTR->window.window,1);
		std::cout << "button_pushed";
	}
		
}
void Gui::EditorMenuGui()
{

}
//
bool Gui::ADDButton(float x, float y, const glm::vec3& color)
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	const float w = 200, h = 100;

	float mycolorR = color.r;
	float mycolorG = color.g;
	float mycolorB = color.b;

	float mouse_pos_x = NATIVEPTR->mouse.x;
	float mouse_pos_y = NATIVEPTR->mouse.y;

	bool is_hovered = (mouse_pos_x >= x && mouse_pos_x <= (x + w) &&
		               mouse_pos_y >= y && mouse_pos_y <= (y + h));

	//std::cout << "Mouse X: " << NATIVEPTR->mouse.x << " Y: " << NATIVEPTR->mouse.y
	//	<< " | Button X: " << x << " Y: " << y << "\n";
	if (is_hovered == true) { mycolorR = color.g + 0.5; }
	
	unsigned int idx = NATIVEPTR->gui_vertex->count;
	if (idx + 6 < MAX_GUI_VERTICES)
	{
		float x2 = x + w;
		float y2 = y + h;

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
			NATIVEPTR->gui_vertex->colorR[idx] = mycolorR;
			NATIVEPTR->gui_vertex->colorG[idx] = mycolorG;
			NATIVEPTR->gui_vertex->colorB[idx] = mycolorB;
			idx++;
		}
		NATIVEPTR->gui_vertex->count = idx;
	}
	return is_hovered && NATIVEPTR->mouse.left_clicked_this_frame;
}
