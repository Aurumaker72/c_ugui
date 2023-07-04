//
// Created by Alex on 02/07/2023.
//

#ifndef RAYLIB_TODO_LIST_C_UGUI_RAYLIB_RENDERER_H
#define RAYLIB_TODO_LIST_C_UGUI_RAYLIB_RENDERER_H

#include "c_ugui.h"

void raylib_load_theme(const char*, const char*);
void raylib_unload();

void raylib_draw_button(t_control, e_visual_state, t_button);

void raylib_draw_togglebutton(t_control, e_visual_state, t_togglebutton);

void raylib_draw_textbox(t_control, e_visual_state, t_textbox);

void raylib_draw_slider(t_control, e_visual_state, t_slider);

void raylib_draw_listbox(t_control, e_visual_state, t_listbox);
void raylib_draw_progressbar(t_control, e_visual_state, t_progressbar);
void raylib_draw_treeview(t_control, e_visual_state, t_treeview);

t_vector2 raylib_measure_text(const char *);

float raylib_listbox_get_item_height();

#endif //RAYLIB_TODO_LIST_C_UGUI_RAYLIB_RENDERER_H
