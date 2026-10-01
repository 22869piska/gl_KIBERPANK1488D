#include "editor.hpp"
#include "..\engine.hpp"

bool BIN_IO::Save(const void* data, size_t size, const char name[])
{
    FILE* f = fopen(name, "wb");//перезапись с нуля
    if (!f) { return false; }

    size_t written = fwrite(data, 1, size, f);
    fclose(f);

    return written == size;
}
bool BIN_IO::SaveAppend(const void* data, size_t size, const char name[])
{
    FILE* f = fopen(name, "ab");//дозапись в конец
    if (!f) { return false; }

    size_t written = fwrite(data, 1, size, f);
    fclose(f);

    return written == size;
}
bool BIN_IO::Load(void* data, size_t size, const char name[],unsigned int OFFSET)
{
    FILE* f = fopen(name, "rb");
    if (!f) {
        perror("LOAD ERROR (File not found?)");
        return false;
    }

    if (fseek(f, OFFSET, SEEK_SET) != 0) {
        perror("LOAD ERROR (fseek failed)");
        fclose(f);
        return false;
    }

    size_t read = fread(data, 1, size, f);
    fclose(f);

    return read == size;
}
//-----------------------------------------------------------------------//
glm::vec2 EDITOR::SnapToGrid(glm::vec2 worldPos)
{
    glm::vec2 snapped;
    // Округляем позицию  сетки
    snapped.x = std::round(worldPos.x / this->gridsize) * this->gridsize;
    snapped.y = std::round(worldPos.y / this->gridsize) * this->gridsize;
    return snapped;
}
void EDITOR::PreviewMode()
{

}
void EDITOR::EditorLogic()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	static auto& GUI = Engine::native_ptr->gui;
     #define              PARAM EDITOR::spawninfo
	static const auto& BLOCK = Engine::native_ptr->block;
	static const auto& LIGHT = Engine::native_ptr->light;
	//INIT ->
	static bool initialized = false;
	if (initialized == false)
	{
		type = block;

		PARAM.pos.x = INFINITY;
		PARAM.pos.y = INFINITY;

		BLOCK->count = 0;
		BLOCK->add(PARAM);
		LIGHT->count = 0;
		LIGHT->add(PARAM);

		NATIVEPTR->mouse.left_clicked_this_frame = false;
		initialized = true;
	}
	
	bool shift_button = false;
	//Swap type (TAB)
	if (glfwGetKey(NATIVEPTR->window.window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		if (NATIVEPTR->input_state.TAB_was_released)//bad
		{
			if      (type == block) { BLOCK->pos_X[0] = INFINITY; BLOCK->pos_Y[0] = INFINITY; }
			else if (type == light) { LIGHT->pos_X[0] = INFINITY; LIGHT->pos_Y[0] = INFINITY; }
			//
			int ntype = static_cast<int>(type) + 1;
			if (ntype > 2) { ntype = 1; }
			type = static_cast<Type>(ntype);
			//

			NATIVEPTR->input_state.TAB_was_released = false;
		}
	}
	else { NATIVEPTR->input_state.TAB_was_released = true; }
	//delete type (NONE)
	if (glfwGetKey(NATIVEPTR->window.window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE) 
	{ 
		BLOCK->pos_X[0] = INFINITY;
		BLOCK->pos_Y[0] = INFINITY;

		LIGHT->pos_X[0] = INFINITY;
		LIGHT->pos_Y[0] = INFINITY;

		shift_button = true;
	}

	if (shift_button == true && NATIVEPTR->mouse.right_is_down == true)
	{
		glm::vec2 cursor_pos = GUI.ScreenToWorldOrtho();

		if      (type == block)
		{
			for (unsigned int i = 1;i < BLOCK->count;i++)
			{
				float half_w = BLOCK->scale_X[i] * 0.5f;
				float half_h = BLOCK->scale_Y[i] * 0.5f;

				float minX = BLOCK->pos_X[i] - half_w;
				float maxX = BLOCK->pos_X[i] + half_w;
				float minY = BLOCK->pos_Y[i] - half_h;
				float maxY = BLOCK->pos_Y[i] + half_h;

				if (cursor_pos.x >= minX && cursor_pos.x <= maxX && cursor_pos.y >= minY && cursor_pos.y <= maxY)
				{
					BLOCK->remove(i);
					break;
				}

			}
		}
		else if (type == light)
		{

		}
	}
	else if(shift_button == false)
	{
		     if (type == block) { SpawnBlockLogic(); }
		else if (type == light) { SpawnLightLogic(); }
	}
	//prev text ->
	char buffer[256];

	sprintf(buffer, "posX: %.2f", PARAM.pos.x);
	NATIVEPTR->gui.ADDText(36, 36, 1600, 1010, buffer, glm::vec3(1, 0, 1));

	sprintf(buffer, "posY: %.2f", PARAM.pos.y);
	NATIVEPTR->gui.ADDText(36, 36, 1600, 1010 - 36, buffer, glm::vec3(1, 0, 1));

	if (type == block)
	{
		sprintf(buffer, "count: %u / %d", BLOCK->count,MAX_BLOCK);
		NATIVEPTR->gui.ADDText(36, 36, 1600, 1010 + 36 , buffer, glm::vec3(0.5, 0.5, 1));

		sprintf(buffer, "texture: %u", PARAM.tex);
		NATIVEPTR->gui.ADDText(36, 36, 1600, 1010 - (36 * 2), buffer, glm::vec3(0.5, 0.5, 1));
	}
	else if (type == light)
	{
		sprintf(buffer, "count: %u / %d", LIGHT->count,MAX_LIGHTS);
		NATIVEPTR->gui.ADDText(36, 36, 1600, 1010 + 36, buffer, glm::vec3(1, 0.5, 0.5));

		sprintf(buffer, "RGB: %.2f,%.2f,%.2f", PARAM.color.r, PARAM.color.g, PARAM.color.b);
		NATIVEPTR->gui.ADDText(36, 36, 1600, 1010 - (36 * 2), buffer, glm::vec3(1, 0.5, 0.5));
	}
	
}
void EDITOR::SpawnBlockLogic()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	static auto& GUI = Engine::native_ptr->gui;
    #define              PARAM EDITOR::spawninfo
	static const auto& BLOCK = Engine::native_ptr->block;

	glm::vec2 cursor_pos = GUI.ScreenToWorldOrtho();	

	//Set param ->
	PARAM.pos = cursor_pos = SnapToGrid(cursor_pos);
	int scroll = static_cast<int>(NATIVEPTR->mouse.scroll_y); if (scroll != 0) // <- scroll TEX PARAM
	{
		int current_tex = static_cast<int>(PARAM.tex) + scroll;
		int max_tex = static_cast<int>(NATIVEPTR->texture.MAX_TEXTURES);

		if (current_tex > max_tex) {
			current_tex = 0;
		}
		else if (current_tex < 0) {
			current_tex = max_tex;
		}
		PARAM.tex = static_cast<decltype(PARAM.tex)>(current_tex);
		NATIVEPTR->mouse.scroll_y = 0.0f;
	}


	//Set prev ->
	BLOCK->pos_X[0] = cursor_pos.x;
	BLOCK->pos_Y[0] = cursor_pos.y;
	BLOCK->texture[0] = PARAM.tex;

	
	
	//Set obj ->
	if (NATIVEPTR->mouse.left_clicked_this_frame && GUI.mouse_over_gui == false){ BLOCK->add(PARAM); }
}
void EDITOR::SpawnLightLogic()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	static auto& GUI = Engine::native_ptr->gui;
    #define              PARAM EDITOR::spawninfo
	static const auto& LIGHT = Engine::native_ptr->light;

	glm::vec2 cursor_pos = GUI.ScreenToWorldOrtho();

	//Set param ->
	PARAM.pos = cursor_pos;

	//Set prev ->
	LIGHT->pos_X[0] = cursor_pos.x;
	LIGHT->pos_Y[0] = cursor_pos.y;
	LIGHT->radius[0] = PARAM.rad;
	LIGHT->intensity[0] = PARAM.intens;
	LIGHT->color_R[0] = PARAM.color.r;
	LIGHT->color_G[0] = PARAM.color.g;
	LIGHT->color_B[0] = PARAM.color.b;

	//Set obj ->
	if (NATIVEPTR->mouse.left_clicked_this_frame && GUI.mouse_over_gui == false) { LIGHT->add(PARAM); }
}
	
