//
// Created by https://github.com/Aurumaker72/
//

#ifndef C_UGUI_H
#define C_UGUI_H

#include <stdint.h>
#include <uchar.h>

typedef enum e_keycodes {
    e_keycodes_backspace = 259,
    e_keycodes_space = 32,
    e_keycodes_left = 263,
    e_keycodes_right = 262,
} e_keycodes;

typedef struct t_vector2 {
    float x, y;
} t_vector2;

typedef struct t_vector2i {
    int32_t x, y;
} t_vector2i;

typedef struct t_rectangle {
    float x, y, width, height;
} t_rectangle;

typedef enum e_visual_state {
    e_visual_state_normal,
    e_visual_state_hovered,
    e_visual_state_active,
    e_visual_state_disabled,
} e_visual_state;

typedef struct t_input {
    t_vector2 mouse_position;
    int32_t is_primary_mouse_button_down;
    int32_t *pressed_chars;
    size_t pressed_chars_length;
    int32_t *pressed_keycodes;
    size_t pressed_keycodes_length;
    t_vector2 mouse_wheel_delta;
} t_input;

typedef struct t_control {
    uint32_t uid;
    t_rectangle rectangle;
    int32_t is_enabled;
} t_control;

typedef struct t_button {
    const char *text;
} t_button;

typedef struct t_togglebutton {
    const char *text;
    int32_t is_checked;
} t_togglebutton;

typedef struct t_textbox {
    char *text;
    size_t text_length;

    int32_t caret_index;
    int32_t selection_start_index;
    int32_t selection_end_index;
} t_textbox;

typedef struct t_slider {
    float value;
} t_slider;

typedef struct t_listbox {
    const char **items;
    size_t items_length;
    uint32_t selected_index;
    float translation;
} t_listbox;

typedef void (*renderer_draw_button)(t_control, e_visual_state, t_button);

typedef void (*renderer_draw_togglebutton)(t_control, e_visual_state, t_togglebutton);

typedef void (*renderer_draw_textbox)(t_control, e_visual_state, t_textbox);

typedef void (*renderer_draw_slider)(t_control, e_visual_state, t_slider);

typedef void (*renderer_draw_listbox)(t_control, e_visual_state, t_listbox);

typedef float (*renderer_listbox_get_item_height)();

typedef t_vector2 (*measure_text)(const char *text);

typedef struct t_renderer {
    measure_text measure_text;
    renderer_draw_button draw_button;
    renderer_draw_togglebutton draw_togglebutton;
    renderer_draw_textbox draw_textbox;
    renderer_draw_slider draw_slider;
    renderer_draw_listbox draw_listbox;
    renderer_listbox_get_item_height listbox_get_item_height;
} t_renderer;

int32_t gui_get_focus_uid();

void gui_begin_frame(t_input, t_renderer *);

int32_t gui_button(t_control, t_button);

int32_t gui_togglebutton(t_control, t_togglebutton);

t_textbox gui_textbox(t_control, t_textbox);

float gui_slider(t_control, t_slider);

t_listbox gui_listbox(t_control, t_listbox);


inline static int32_t is_vector2_inside(t_vector2 vector2, t_rectangle rectangle) {
    return vector2.x > rectangle.x && vector2.x < rectangle.x + rectangle.width && vector2.y > rectangle.y &&
           vector2.y < rectangle.y + rectangle.height;
}

#endif //C_UGUI_H
