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
	const float gx = (std::max)(spawninfo.scale.x, 1.0f);
	const float gy = (std::max)(spawninfo.scale.y, 1.0f);

	const float hx = spawninfo.scale.x * 0.5f, hy = spawninfo.scale.y * 0.5f;
	return {
		std::round((worldPos.x - hx) / gx) * gx + hx,
		std::round((worldPos.y - hy) / gy) * gy + hy
	};
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
	static auto& INPTST = Engine::native_ptr->input_state;
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

		NATIVEPTR->input_state.mouse.left_clicked_this_frame = false;
		initialized = true;
	}
	bool shift_button = INPTST.LSHIFT;
	{
		//WASD cameramov параша
		if (INPTST.keys_alpha[IDX_W] == 1) { NATIVEPTR->camera_posY += 5.0f; }
		if (INPTST.keys_alpha[IDX_A] == 1) { NATIVEPTR->camera_posX -= 5.0f; }
		if (INPTST.keys_alpha[IDX_S] == 1) { NATIVEPTR->camera_posY -= 5.0f; }
		if (INPTST.keys_alpha[IDX_D] == 1) { NATIVEPTR->camera_posX += 5.0f; }
		//camerazoom
		if (INPTST.EQUAL != GLFW_RELEASE) { NATIVEPTR->camera_zoom += 0.01f; }
		if (INPTST.MINUS != GLFW_RELEASE) { NATIVEPTR->camera_zoom -= 0.01f; }
		//Swap type (TAB)
		if (INPTST.TAB == GLFW_PRESS)
		{
			if (INPTST.TAB_was_released == true)//bad
			{
				if (type == block) { BLOCK->pos_X[0] = INFINITY; BLOCK->pos_Y[0] = INFINITY; }
				else if (type == light) { LIGHT->pos_X[0] = INFINITY; LIGHT->pos_Y[0] = INFINITY; }
				//
				int ntype = static_cast<int>(type) + 1;
				if (ntype > 2) { ntype = 1; }
				type = static_cast<Type>(ntype);
				//

				INPTST.TAB_was_released = false;
			}
		}
		else { INPTST.TAB_was_released = true; }
		//delete type (SHIFT)
		if (shift_button != GLFW_RELEASE)
		{
			BLOCK->pos_X[0] = INFINITY;
			BLOCK->pos_Y[0] = INFINITY;

			LIGHT->pos_X[0] = INFINITY;
			LIGHT->pos_Y[0] = INFINITY;
		}
		
	}
	//delete&&info ->
	//if (shift_button == true )
	{
		 unsigned int index_block = 0;
		 unsigned int index_light = 0;
		 glm::vec2 cursor_pos = GUI.ScreenToWorldOrtho();

		 if (type == block)
		 {
			bool cursor_on_obj = false;

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
					index_block = i;
	
					if(INPTST.mouse.right_is_down == true && shift_button == true){ BLOCK->remove(i); }
					break;
				}

			}
		 }
		 else if (type == light)
		 {
			 bool cursor_on_obj = false;

			 for (unsigned int i = 1; i < LIGHT->count; i++)
			 {
				 float dx = cursor_pos.x - LIGHT->pos_X[i];
				 float dy = cursor_pos.y - LIGHT->pos_Y[i];
				 float r = LIGHT->radius[i];

				 if (dx * dx + dy * dy <= r * r)
				 {
					 index_light = i;
					 if (INPTST.mouse.right_is_down == true && shift_button == true) { LIGHT->remove(i); }
					 break;
				 }
			 }
		 }

		 this->index_block_touch = index_block;
		 this->index_light_touch = index_light;
	
	}
	if(shift_button == false)
	{
		     if (type == block) { SpawnBlockLogic(); }
		else if (type == light) { SpawnLightLogic(); }
	}
	//prev text ->
	ShowDebugInfo(type);
	
}
void EDITOR::SpawnBlockLogic()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	static auto& GUI = Engine::native_ptr->gui;
    #define              PARAM EDITOR::spawninfo
	static const auto& BLOCK = Engine::native_ptr->block;
	static auto& INPTST = Engine::native_ptr->input_state;

	glm::vec2 cursor_pos = GUI.ScreenToWorldOrtho();
	cursor_pos = SnapToGrid(cursor_pos);
	//Set param ->
	int scroll = static_cast<int>(INPTST.mouse.scroll_y); if (scroll != 0) // <- scroll PARAM
	{
		if (INPTST.LCTRL != GLFW_RELEASE)	//scaleXY->	
		{ 
			const float old_X = 50.f;
			const float old_Y = 50.f;

			if (scroll == 1)
			{
				if (PARAM.scale.x == old_X) { PARAM.scale.x *= 0.5f; }
				else { PARAM.scale.x *= 2.f; }
			}
			if (scroll == -1)
			{
				if (PARAM.scale.y == old_Y) { PARAM.scale.y *= 0.5f; }
				else { PARAM.scale.y *= 2.f; }
			}
		}
		if (INPTST.keys_alpha[IDX_F] != GLFW_RELEASE)
		{
			flag_bit_offset += scroll;
			if (this->flag_bit_offset > 31) { this->flag_bit_offset = 31; }
			else if (this->flag_bit_offset < 0) { this->flag_bit_offset = 0; }
		}
		else //tex->
		{
			int current_tex = static_cast<int>(PARAM.tex) + scroll;
			int max_tex = static_cast<int>(NATIVEPTR->texture.MAX_TEXTURES - 1);

			if (current_tex > max_tex) { current_tex = 0; }
			else if (current_tex < 0) { current_tex = max_tex; }

			PARAM.tex = static_cast<decltype(PARAM.tex)>(current_tex);	
		}
		INPTST.mouse.scroll_y = 0.0f;
	}
	{//swap bit
		static int prev_space = 0;
		if (INPTST.SPACE == GLFW_PRESS && prev_space == 0)
		{
			PARAM.flag ^= (1U << flag_bit_offset);
			prev_space = 1;
		}
		else if(INPTST.SPACE == GLFW_RELEASE){ prev_space = 0; }
	}
	//param только после вычислений ->
	PARAM.pos = SnapToGrid(cursor_pos);
	//Set prev ->
	{
		BLOCK->pos_X[0] = PARAM.pos.x;       
		BLOCK->pos_Y[0] = PARAM.pos.y;
		BLOCK->scale_X[0] = PARAM.scale.x; 
		BLOCK->scale_Y[0] = PARAM.scale.y;
		BLOCK->texture[0] = PARAM.tex;
		BLOCK->flag[0] = PARAM.flag;
	}
	//Set obj ->
	if (GUI.mouse_over_gui == false)
	{
		if(INPTST.mouse.left_clicked_this_frame == true){  BLOCK->add(PARAM);  }
		else if (INPTST.mouse.right_is_down == true && this->index_block_touch == 0){ BLOCK->add(PARAM); }
	}
}
void EDITOR::SpawnLightLogic()
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	static auto& GUI = Engine::native_ptr->gui;
    #define              PARAM EDITOR::spawninfo
	static const auto& LIGHT = Engine::native_ptr->light;
	static auto& INPTST = Engine::native_ptr->input_state;

	glm::vec2 cursor_pos = GUI.ScreenToWorldOrtho();

	//Set param ->
	PARAM.pos = cursor_pos;
	int scroll = static_cast<int>(INPTST.mouse.scroll_y);if (scroll != 0)
	{
		bool enterinif = false;
		const float colorincadd = 0.05f;

		if (glfwGetKey(NATIVEPTR->window.window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		{
			enterinif = true;
			PARAM.intens += scroll * 0.01f;
			if (PARAM.intens < 0.f) { PARAM.intens = 0.f; }
			else if (PARAM.intens > 1.0f) { PARAM.intens = 1.f; }
		}
		if (glfwGetKey(NATIVEPTR->window.window, GLFW_KEY_R) == GLFW_PRESS)
		{
			enterinif = true;
			PARAM.color.r += scroll * colorincadd;
			if (PARAM.color.r < 0.f) { PARAM.color.r = 0.f; }
			else if (PARAM.color.r > 1.0f) { PARAM.color.r = 1.f; }
		}
		if (glfwGetKey(NATIVEPTR->window.window, GLFW_KEY_G) == GLFW_PRESS)
		{
			enterinif = true;
			PARAM.color.g += scroll * colorincadd;
			if (PARAM.color.g < 0.f) { PARAM.color.g = 0.f; }
			else if (PARAM.color.g > 1.0f) { PARAM.color.g = 1.f; }
		}
		if (glfwGetKey(NATIVEPTR->window.window, GLFW_KEY_B) == GLFW_PRESS)
		{
			enterinif = true;
			PARAM.color.b += scroll * colorincadd;
			if (PARAM.color.b < 0.f) { PARAM.color.b = 0.f; }
			else if (PARAM.color.b > 1.0f) { PARAM.color.b = 1.f; }
		}

		if (enterinif == false) { PARAM.rad += scroll * 10.f; }
	}

	//Set prev ->
	LIGHT->pos_X[0] = cursor_pos.x;
	LIGHT->pos_Y[0] = cursor_pos.y;
	LIGHT->radius[0] = PARAM.rad;
	LIGHT->intensity[0] = PARAM.intens;
	LIGHT->color_R[0] = PARAM.color.r;
	LIGHT->color_G[0] = PARAM.color.g;
	LIGHT->color_B[0] = PARAM.color.b;

	//Set obj ->
	if (INPTST.mouse.left_clicked_this_frame && GUI.mouse_over_gui == false) { LIGHT->add(PARAM); }
}
void EDITOR::ShowDebugInfo(Type type_obj)
{
	static const auto& NATIVEPTR = Engine::native_ptr;
	static auto& GUI = Engine::native_ptr->gui;
#define              PARAM EDITOR::spawninfo
	static const auto& BLOCK = Engine::native_ptr->block;
	static const auto& LIGHT = Engine::native_ptr->light;

	const bool shift_button = Engine::native_ptr->input_state.LSHIFT;
	const unsigned int index_block = this->index_block_touch;
	const unsigned int index_light = this->index_light_touch;

	char buffer[256];
	const unsigned int wOFFSET_W = 10;
	const unsigned int wOFFSET_H = 1075;
	const unsigned int sWH = 42;
    const unsigned int offset_stride = 30;
	unsigned int stride_count = 1;

	char flag_buf[33];
	int  target_bit = this->flag_bit_offset;
	for (int i = 31, n = 31; i >= 0; i--, n--) { flag_buf[n] = (PARAM.flag & (1U << i)) ? '1' : '0'; }
	flag_buf[32] = '\0';

	if (shift_button == true) //delete/info
	{
		if (index_block != 0)//если индекс == 0; значит мы не попали курсором по блоку или вовсе не проверяли условие (говнокод);
		{
			sprintf(buffer, "posX: %.2f", BLOCK->pos_X[index_block]);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1.0, 1.0, 1.0));stride_count++;
			sprintf(buffer, "posY: %.2f", BLOCK->pos_Y[index_block]);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1.0, 1.0, 1.0));stride_count++;

			//->
			sprintf(buffer, "index: %u", index_block);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1.0, 1.0, 1.0));stride_count++;
			sprintf(buffer, "texID: %u", BLOCK->texture[index_block]);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1.0, 1.0, 1.0));stride_count++;
			//->

			//->
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), flag_buf, glm::vec3(1, 1, 1),
			                       target_bit, 1, glm::vec3(1.0f, 1.0f, 0.0f));stride_count++;
			
		}
		else if (index_light != 0)
		{
			sprintf(buffer, "posX: %.2f", LIGHT->pos_X[index_light]);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1.0, 1.0, 1.0));stride_count++;
			sprintf(buffer, "posY: %.2f", LIGHT->pos_X[index_light]);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1.0, 1.0, 1.0));stride_count++;
			//->
			sprintf(buffer, "index: %u", index_light);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1.0, 1.0, 1.0));stride_count++;

			sprintf(buffer, "RGB: %.2f,%.2f,%.2f", PARAM.color.r, PARAM.color.g, PARAM.color.b);
			NATIVEPTR->gui.ADDText(sWH * 0.75, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 1, 1));stride_count++;

			sprintf(buffer, "rad: %.2f", PARAM.rad);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 1, 1));stride_count++;

			sprintf(buffer, "intens: %.2f", PARAM.intens);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 1, 1));stride_count++;
		}
	}
	else
	{
		sprintf(buffer, "posX: %.2f", PARAM.pos.x);
		NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - offset_stride, buffer, glm::vec3(1, 0, 1));stride_count++;
		sprintf(buffer, "posY: %.2f", PARAM.pos.y);
		NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 0, 1));stride_count++;

		//campos->
		sprintf(buffer, "camXY: %.1f,%.1f,%.1f", NATIVEPTR->camera_posX, NATIVEPTR->camera_posY,NATIVEPTR->camera_zoom);
		NATIVEPTR->gui.ADDText(sWH * 0.85, sWH, 1500, 10, buffer, glm::vec3(1, 1, 1));

		sprintf(buffer, "ZOOM: %.1f",NATIVEPTR->camera_zoom);
		NATIVEPTR->gui.ADDText(sWH * 0.85, sWH, 1500, 10 + (sWH * 1.25), buffer, glm::vec3(1, 1, 1));


		if (type == block)
		{
			sprintf(buffer, "count: %u / %d", BLOCK->count, MAX_BLOCK);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(0.5, 0.5, 1));stride_count++;

			sprintf(buffer, "texture: %u", PARAM.tex);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(0.5, 0.5, 1));stride_count++;

			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), flag_buf, glm::vec3(0.5, 0.5, 1),
				                   target_bit, 1, glm::vec3(1.0f, 1.0f, 0.0f));stride_count++;
		}
		else if (type == light)
		{
			sprintf(buffer, "count: %u / %d", LIGHT->count, MAX_LIGHTS);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 0.5, 0.5));stride_count++;

			sprintf(buffer, "RGB: %.2f,%.2f,%.2f", PARAM.color.r, PARAM.color.g, PARAM.color.b);
			NATIVEPTR->gui.ADDText(sWH * 0.75, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 0.5, 0.5));stride_count++;

			sprintf(buffer, "rad: %.2f", PARAM.rad);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 0.5, 0.5));stride_count++;

			sprintf(buffer, "intens: %.2f", PARAM.intens);
			NATIVEPTR->gui.ADDText(sWH, sWH, wOFFSET_W, wOFFSET_H - (offset_stride * stride_count), buffer, glm::vec3(1, 0.5, 0.5));stride_count++;

		}
	}
	
}
