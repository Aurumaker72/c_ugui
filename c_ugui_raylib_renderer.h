//
// Created by Alex on 02/07/2023.
//

#ifndef RAYLIB_TODO_LIST_C_UGUI_RAYLIB_RENDERER_H
#define RAYLIB_TODO_LIST_C_UGUI_RAYLIB_RENDERER_H

#include "c_ugui.h"
#include "raylib.h"

typedef struct t_visual_style {
    Texture2D texture;
    Font font;
    NPatchInfo raised_frames[4];
    NPatchInfo edit_frames[4];
    NPatchInfo list_frames[4];
    NPatchInfo list_selected_frames[4];
    NPatchInfo slider_tracks[4];
    Rectangle slider_horizontal_thumbs[4];
    Rectangle slider_vertical_thumbs[4];
    NPatchInfo progressbar_bars[4];
} t_visual_style;

void raylib_unload_visual_style(void);

void raylib_set_visual_style(t_visual_style);

void raylib_draw_button(t_control, e_visual_state, t_button);

void raylib_draw_togglebutton(t_control, e_visual_state, t_togglebutton);

void raylib_draw_textbox(t_control, e_visual_state, t_textbox);

void raylib_draw_slider(t_control, e_visual_state, t_slider);

void raylib_draw_listbox(t_control, e_visual_state, t_listbox);

void raylib_draw_progressbar(t_control, e_visual_state, t_progressbar);

void raylib_draw_treeview(t_control, e_visual_state, t_treeview);

void raylib_draw_treeview_node(t_control, e_visual_state, t_node, size_t, size_t);

t_vector2 raylib_measure_text(const char *);

float raylib_listbox_get_item_height();

#endif //RAYLIB_TODO_LIST_C_UGUI_RAYLIB_RENDERER_H
