//
// Created by Alex on 02/07/2023.
//

#include "c_ugui_raylib_renderer.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "c_ugui.h"

t_visual_style visual_style;

void slice(const char *str, char *result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
}

int32_t imin2(int32_t a, int32_t b) {
    if (a > b) return b;
    return a;
}

int32_t imax2(int32_t a, int32_t b) {
    if (a > b) return a;
    return b;
}

Rectangle inflate_rectangle(Rectangle rectangle, float value) {
    return (Rectangle) {
            rectangle.x - value,
            rectangle.y - value,
            rectangle.width + value * 2,
            rectangle.height + value * 2,
    };
}

t_vector2 raylib_measure_text(const char *text) {
    Vector2 size = MeasureTextEx(visual_style.font, text, visual_style.font.baseSize, 0.0f);
    return (t_vector2) {size.x, size.y};
}


float caret_x_from_index(const char *text, int32_t caret_index) {
    char *sliced_str = (char *) calloc(strlen(text), sizeof(char));

    slice(text, sliced_str, 0, caret_index);

    float caret_x = raylib_measure_text(sliced_str).x;

    free(sliced_str);
    return caret_x;
}

void raylib_draw_button(t_control control, e_visual_state visual_state, t_button button) {

    NPatchInfo patch_info = visual_style.raised_frames[visual_state];

    DrawTextureNPatch(visual_style.texture, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    Vector2 text_bounds = MeasureTextEx(visual_style.font, button.text, visual_style.font.baseSize, 0.0f);

    DrawTextEx(visual_style.font, button.text, (Vector2) {
            control.rectangle.x + control.rectangle.width / 2 - text_bounds.x / 2,
            control.rectangle.y + control.rectangle.height / 2 - text_bounds.y / 2,
    }, visual_style.font.baseSize, 0.0f, control.is_enabled ? BLACK : (Color) {
            160,
            160,
            160,
            255
    });
}

void raylib_draw_togglebutton(t_control control, e_visual_state visual_state, t_togglebutton togglebutton) {
    raylib_draw_button(control, visual_state, (t_button) {
            .text = togglebutton.text
    });
}

void raylib_draw_textbox(t_control control, e_visual_state visual_state, t_textbox textbox) {

    NPatchInfo patch_info = visual_style.edit_frames[visual_state];

    DrawTextureNPatch(visual_style.texture, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    Vector2 text_bounds = MeasureTextEx(visual_style.font, textbox.text, 14.0f, 0.0f);

    DrawTextEx(visual_style.font, textbox.text, (Vector2) {
            control.rectangle.x,
            control.rectangle.y + control.rectangle.height / 2 - text_bounds.y / 2,
    }, visual_style.font.baseSize, 0.0f, control.is_enabled ? BLACK : (Color) {
            160,
            160,
            160,
            255
    });

    // we dont draw the caret or selection when a textbox is unfocused
    if (gui_get_focus_uid() != control.uid) {
        return;
    }

    float caret_x = caret_x_from_index(textbox.text, textbox.caret_index);
    float selection_start_x = caret_x_from_index(textbox.text, textbox.selection_start_index);
    float selection_end_x = caret_x_from_index(textbox.text, textbox.selection_end_index);

    // we need to ensure that start x is lower than end x
    if (selection_start_x > selection_end_x) {
        float start = selection_start_x;
        selection_start_x = selection_end_x;
        selection_end_x = start;
    }

    DrawTexturePro(visual_style.texture, (Rectangle) {
            20,
            23,
            1,
            1
    }, (Rectangle) {
            control.rectangle.x + caret_x,
            control.rectangle.y,
            1,
            control.rectangle.height
    }, (Vector2) {0}, 0.0f, WHITE);

    DrawTexturePro(visual_style.texture, (Rectangle) {
            21,
            23,
            1,
            1
    }, (Rectangle) {
            control.rectangle.x + selection_start_x,
            control.rectangle.y,
            selection_end_x - selection_start_x,
            control.rectangle.height
    }, (Vector2) {0}, 0.0f, WHITE);

    BeginScissorMode(control.rectangle.x + selection_start_x, control.rectangle.y, selection_end_x - selection_start_x,
                     control.rectangle.height);
    DrawTextEx(visual_style.font, textbox.text, (Vector2) {
            control.rectangle.x,
            control.rectangle.y + control.rectangle.height / 2 - text_bounds.y / 2,
    }, visual_style.font.baseSize, 0.0f, WHITE);
    EndScissorMode();
}

void raylib_draw_slider(t_control control, e_visual_state visual_state, t_slider slider) {
    const int32_t is_horizontal = control.rectangle.width > control.rectangle.height;
    const float track_thickness = 4.0f;

    Rectangle track_rectangle;

    if (is_horizontal) {
        track_rectangle = (Rectangle) {
                .x = control.rectangle.x,
                .y = control.rectangle.y + control.rectangle.height / 2 - track_thickness / 2,
                .width = control.rectangle.width,
                .height = track_thickness,
        };
    } else {
        track_rectangle = (Rectangle) {
                .x = control.rectangle.x + control.rectangle.width / 2 - track_thickness / 2,
                .y = control.rectangle.y,
                .width = track_thickness,
                .height = control.rectangle.height,
        };
    }
    SetTextureFilter(visual_style.texture, TEXTURE_FILTER_POINT);
    DrawTextureNPatch(visual_style.texture, visual_style.slider_tracks[visual_state], track_rectangle, (Vector2) {0},
                      0.0f, WHITE);


    Rectangle thumb_dest_rectangle;

    if (is_horizontal) {
        thumb_dest_rectangle = (Rectangle) {
                control.rectangle.x + (control.rectangle.width * slider.value) - visual_style.slider_thumb_size.x / 2,
                control.rectangle.y + control.rectangle.height / 2 - visual_style.slider_thumb_size.y / 2,
                visual_style.slider_thumb_size.x,
                visual_style.slider_thumb_size.y
        };
    } else {
        thumb_dest_rectangle = (Rectangle) {
                control.rectangle.x + control.rectangle.width / 2 - visual_style.slider_thumb_size.y / 2,
                control.rectangle.y + (control.rectangle.height * slider.value) - visual_style.slider_thumb_size.x / 2,
                visual_style.slider_thumb_size.y,
                visual_style.slider_thumb_size.x
        };
    }

    DrawTexturePro(visual_style.texture, is_horizontal ? visual_style.slider_horizontal_thumbs[visual_state]
                                                       : visual_style.slider_vertical_thumbs[visual_state],
                   thumb_dest_rectangle, (Vector2) {0}, 0.0f, WHITE);
}

void raylib_draw_listbox(t_control control, e_visual_state visual_state, t_listbox listbox) {

    DrawTextureNPatch(visual_style.texture, visual_style.list_frames[visual_state], (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    if (!listbox.items)
        return;

    float list_height = visual_style.list_item_height * listbox.items_length;

    size_t index_begin =
            (listbox.translation * (list_height - control.rectangle.height)) / visual_style.list_item_height;
    size_t index_end =
            ((listbox.translation * list_height) + control.rectangle.height) / visual_style.list_item_height;

    // we extend the range by one,
    // then clamp it to the actual indicies, as not to read oob
    index_begin = imax2(index_begin - 1, 0);
    index_end = imin2(index_end + 1, listbox.items_length);

    BeginScissorMode(control.rectangle.x, control.rectangle.y, control.rectangle.width, control.rectangle.height);
    for (int i = index_begin; i < index_end; ++i) {


        float y = (visual_style.list_item_height * i) -
                  (listbox.translation *
                   ((visual_style.list_item_height * listbox.items_length) - control.rectangle.height));

        if (listbox.selected_index == i) {
            DrawTextureNPatch(visual_style.texture, visual_style.list_selected_frames[visual_state],
                              inflate_rectangle((Rectangle) {
                                      .x = control.rectangle.x,
                                      .y = control.rectangle.y + y,
                                      .width = control.rectangle.width,
                                      .height = visual_style.list_item_height,
                              }, -2.0f), (Vector2) {0}, 0.0f, WHITE);
        }

        Rectangle item_rectangle = (Rectangle) {
                control.rectangle.x,
                control.rectangle.y + y,
                control.rectangle.width,
                visual_style.list_item_height
        };

        Vector2 text_bounds = MeasureTextEx(visual_style.font, listbox.items[i], visual_style.font.baseSize, 0.0f);

        Color text_color = visual_style.list_text_color[
                control.is_enabled ? (listbox.selected_index == i ? e_visual_state_active : e_visual_state_normal)
                                   : e_visual_state_disabled
        ];

        DrawTextEx(visual_style.font, listbox.items[i], (Vector2) {
                (item_rectangle.x) + visual_style.list_content_padding.x,
                (item_rectangle.y + item_rectangle.height / 2 - text_bounds.y / 2) +
                visual_style.list_content_padding.y,
        }, visual_style.font.baseSize, 0.0f, text_color);

    }
    EndScissorMode();
}

void raylib_draw_progressbar(t_control control, e_visual_state visual_state, t_progressbar progress_bar) {
    DrawTextureNPatch(visual_style.texture, visual_style.raised_frames[visual_state], (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    DrawTextureNPatch(visual_style.texture, visual_style.progressbar_bars[visual_state], inflate_rectangle((Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width * progress_bar.progress,
            .height = control.rectangle.height,
    }, -1), (Vector2) {0}, 0.0f, WHITE);
}

void raylib_draw_treeview(t_control control, e_visual_state visual_state, t_treeview treeview) {
    raylib_draw_listbox(control, visual_state, (t_listbox) {0});
}

void
raylib_draw_treeview_node(t_control control, e_visual_state visual_state, t_node node, size_t index, size_t subdepth) {
    float x = control.rectangle.x + (visual_style.list_item_height * subdepth);
    float y = control.rectangle.y + (visual_style.list_item_height * index);

    Color text_color = visual_style.list_text_color[
            control.is_enabled ? (node.is_selected ? e_visual_state_active : e_visual_state_normal)
                               : e_visual_state_disabled
    ];
    Vector2 text_bounds = MeasureTextEx(visual_style.font, node.text, visual_style.font.baseSize, 0.0f);


    Rectangle node_rectangle = (Rectangle) {
            .x = x + visual_style.list_content_padding.x,
            .y = y,
            .width = text_bounds.x + visual_style.list_content_padding.x * 2,
            .height = visual_style.list_item_height,
    };

    if (node.is_selected) {
        DrawTextureNPatch(visual_style.texture, visual_style.list_selected_frames[visual_state], node_rectangle,
                          (Vector2) {0}, 0.0f, WHITE);
    }

    DrawTextEx(visual_style.font, node.text, (Vector2) {
            node_rectangle.x + visual_style.list_content_padding.x,
            node_rectangle.y + node_rectangle.height / 2 - text_bounds.y / 2
    }, visual_style.font.baseSize, 0.0f, text_color);
}

void raylib_set_visual_style(t_visual_style _visual_style) {
    visual_style = _visual_style;
}

void raylib_unload_visual_style(void) {
    UnloadTexture(visual_style.texture);
    UnloadFont(visual_style.font);
}

void raylib_clear() {
    ClearBackground(visual_style.background_color);
}

float raylib_listbox_get_item_height() {
    return visual_style.list_item_height;
}
