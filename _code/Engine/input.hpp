#pragma once
#define IDX_A 0
#define IDX_B 1
#define IDX_C 2
#define IDX_D 3
#define IDX_E 4
#define IDX_F 5
#define IDX_G 6
#define IDX_H 7
#define IDX_I 8
#define IDX_J 9
#define IDX_K 10
#define IDX_L 11
#define IDX_M 12
#define IDX_N 13
#define IDX_O 14
#define IDX_P 15
#define IDX_Q 16
#define IDX_R 17
#define IDX_S 18
#define IDX_T 19
#define IDX_U 20
#define IDX_V 21
#define IDX_W 22
#define IDX_X 23
#define IDX_Y 24
#define IDX_Z 25

#define IDX_0 0
#define IDX_1 1
#define IDX_2 2
#define IDX_3 3
#define IDX_4 4
#define IDX_5 5
#define IDX_6 6
#define IDX_7 7
#define IDX_8 8
#define IDX_9 9
struct InputState {
    // Для букв A-Z
    bool keys_alpha[26] = { false };
    // Для цифр 0-9 над буквами
    bool keys_num[10] = { false };
    // Стрелки
    bool UP = false, DOWN = false, LEFT = false, RIGHT = false;

    bool ESCAPE = false;
    bool ENTER = false;
    bool BACKSPACE = false;
    bool SPACE = false;
    bool TAB = false;
    bool CAPS_LOCK = false;
    bool EQUAL = false;
    bool MINUS = false;

    // Модификаторы
    bool LSHIFT = false, RSHIFT = false;
    bool LCTRL = false, RCTRL = false;
    bool LALT = false, RALT = false;
    //->
    bool TAB_was_released = false;

    struct MouseState {
        float cursor_pos_in_worldX = 0.f;
        float cursor_pos_in_worldY = 0.f;
        float x = 0.0f;
        float y = 0.0f;
        float scroll_x = 0.0f;
        float scroll_y = 0.0f;
        bool left_is_down = false;
        bool left_clicked_this_frame = false;
        bool right_is_down = false;
        bool right_clicked_this_frame = false;
    };MouseState mouse;
    void InputClear();
};





