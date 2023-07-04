//
// Created by Alex on 02/07/2023.
//

#include "c_ugui_raylib_renderer.h"
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "c_ugui.h"

Texture2D atlas;
Font font;

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
    Vector2 size = MeasureTextEx(font, text, 14.0f, 0.0f);
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

    NPatchInfo patch_info;

    if (visual_state == e_visual_state_normal) {
        patch_info = (NPatchInfo) {(Rectangle) {1, 1, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    } else if (visual_state == e_visual_state_hovered) {
        patch_info = (NPatchInfo) {(Rectangle) {1, 12, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    } else if (visual_state == e_visual_state_active) {
        patch_info = (NPatchInfo) {(Rectangle) {1, 23, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    } else {
        patch_info = (NPatchInfo) {(Rectangle) {1, 34, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    }

    DrawTextureNPatch(atlas, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    Vector2 text_bounds = MeasureTextEx(font, button.text, font.baseSize, 0.0f);

    DrawTextEx(font, button.text, (Vector2) {
            control.rectangle.x + control.rectangle.width / 2 - text_bounds.x / 2,
            control.rectangle.y + control.rectangle.height / 2 - text_bounds.y / 2,
    }, font.baseSize, 0.0f, control.is_enabled ? BLACK : (Color) {
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

    NPatchInfo patch_info;

    if (visual_state == e_visual_state_normal) {
        patch_info = (NPatchInfo) {(Rectangle) {20, 1, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    } else if (visual_state == e_visual_state_hovered) {
        patch_info = (NPatchInfo) {(Rectangle) {20, 6, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    } else if (visual_state == e_visual_state_active) {
        patch_info = (NPatchInfo) {(Rectangle) {20, 11, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    } else {
        patch_info = (NPatchInfo) {(Rectangle) {20, 16, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    }

    DrawTextureNPatch(atlas, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    Vector2 text_bounds = MeasureTextEx(font, textbox.text, 14.0f, 0.0f);

    DrawTextEx(font, textbox.text, (Vector2) {
            control.rectangle.x,
            control.rectangle.y + control.rectangle.height / 2 - text_bounds.y / 2,
    }, font.baseSize, 0.0f, control.is_enabled ? BLACK : (Color) {
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

    DrawTexturePro(atlas, (Rectangle) {
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

    DrawTexturePro(atlas, (Rectangle) {
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
    DrawTextEx(font, textbox.text, (Vector2) {
            control.rectangle.x,
            control.rectangle.y + control.rectangle.height / 2 - text_bounds.y / 2,
    }, font.baseSize, 0.0f, WHITE);
    EndScissorMode();
}

void raylib_draw_slider(t_control control, e_visual_state visual_state, t_slider slider) {
    const float track_thickness = 4.0f;
    const float thumb_width = 11.0f; // dimensions are 11x9, but for some reason raylib misses the last row of pixels
    const float thumb_height = 19.0f;
    const int32_t is_horizontal = control.rectangle.width > control.rectangle.height;

    NPatchInfo patch_info = (NPatchInfo) {(Rectangle) {36, 1, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};

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
    DrawTextureNPatch(atlas, patch_info, track_rectangle, (Vector2) {0}, 0.0f, WHITE);


    Rectangle thumb_src_rectangle;

    if (is_horizontal) {
        if (visual_state == e_visual_state_normal) {
            thumb_src_rectangle = (Rectangle) {40, 0, 11, 19};
        } else if (visual_state == e_visual_state_hovered) {
            thumb_src_rectangle = (Rectangle) {40, 19, 11, 19};
        } else if (visual_state == e_visual_state_active) {
            thumb_src_rectangle = (Rectangle) {40, 38, 11, 19};
        } else {
            thumb_src_rectangle = (Rectangle) {40, 76, 11, 19};
        }
    } else {
        if (visual_state == e_visual_state_normal) {
            thumb_src_rectangle = (Rectangle) {53, 0, 19, 11};
        } else if (visual_state == e_visual_state_hovered) {
            thumb_src_rectangle = (Rectangle) {53, 11, 19, 11};
        } else if (visual_state == e_visual_state_active) {
            thumb_src_rectangle = (Rectangle) {53, 22, 19, 11};
        } else {
            thumb_src_rectangle = (Rectangle) {53, 44, 19, 11};
        }
    }


    Rectangle thumb_dest_rectangle;

    if (is_horizontal) {
        thumb_dest_rectangle = (Rectangle) {
                control.rectangle.x + (control.rectangle.width * slider.value) - thumb_width / 2,
                control.rectangle.y + control.rectangle.height / 2 - thumb_height / 2,
                thumb_width,
                thumb_height
        };
    } else {
        thumb_dest_rectangle = (Rectangle) {
                control.rectangle.x + control.rectangle.width / 2 - thumb_height / 2 -
                1,// TODO: this alignment hack exists due to raylib incorrectly drawing the thumb image
                control.rectangle.y + (control.rectangle.height * slider.value) - thumb_width / 2,
                thumb_height,
                thumb_width
        };
    }
    DrawTexturePro(atlas, thumb_src_rectangle, thumb_dest_rectangle, (Vector2) {0}, 0.0f, WHITE);
}

float raylib_listbox_get_item_height() {
    return font.baseSize + 4.0f;
}

void raylib_draw_listbox(t_control control, e_visual_state visual_state, t_listbox listbox) {
    const float padding = 2.0f;
    NPatchInfo patch_info = (NPatchInfo) {(Rectangle) {36, 6, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};

    DrawTextureNPatch(atlas, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    if (!listbox.items)
        return;

    float list_height = raylib_listbox_get_item_height() * listbox.items_length;

    size_t index_begin =
            (listbox.translation * (list_height - control.rectangle.height)) / raylib_listbox_get_item_height();
    size_t index_end =
            ((listbox.translation * list_height) + control.rectangle.height) / raylib_listbox_get_item_height();

    // we extend the range by one,
    // then clamp it to the actual indicies, as not to read oob
    index_begin = imax2(index_begin - 1, 0);
    index_end = imin2(index_end + 1, listbox.items_length);

    BeginScissorMode(control.rectangle.x, control.rectangle.y, control.rectangle.width, control.rectangle.height);
    for (int i = index_begin; i < index_end; ++i) {

        Color text_color;
        if (control.is_enabled) {
            text_color = listbox.selected_index == i ? WHITE : BLACK;
        } else {
            text_color = (Color) {109, 109, 109, 255};
        }
        float y = (raylib_listbox_get_item_height() * i) -
                  (listbox.translation *
                   ((raylib_listbox_get_item_height() * listbox.items_length) - control.rectangle.height));

        if (listbox.selected_index == i) {
            NPatchInfo patch_info = (NPatchInfo) {(Rectangle) {34, 11, 5, 5}, 2, 2, 2, 2, NPATCH_NINE_PATCH};

            DrawTextureNPatch(atlas, patch_info, inflate_rectangle((Rectangle) {
                    .x = control.rectangle.x,
                    .y = control.rectangle.y + y,
                    .width = control.rectangle.width,
                    .height = raylib_listbox_get_item_height(),
            }, -2.0f), (Vector2) {0}, 0.0f, WHITE);
        }

        Rectangle item_rectangle = (Rectangle) {
                control.rectangle.x,
                control.rectangle.y + y,
                control.rectangle.width,
                raylib_listbox_get_item_height()
        };

        Vector2 text_bounds = MeasureTextEx(font, listbox.items[i], font.baseSize, 0.0f);

        DrawTextEx(font, listbox.items[i], (Vector2) {
                item_rectangle.x + 4.0f,
                item_rectangle.y + item_rectangle.height / 2 - text_bounds.y / 2,
        }, font.baseSize, 0.0f, text_color);

    }
    EndScissorMode();
}

void raylib_load_theme(const char *atlas_path, const char *font_path) {
    atlas = LoadTexture(atlas_path);
    font = LoadFontEx(font_path, 14, NULL, 250);
}

void raylib_unload() {
    UnloadTexture(atlas);
    UnloadFont(font);
}

void raylib_draw_progressbar(t_control control, e_visual_state visual_state, t_progressbar progress_bar) {
    NPatchInfo patch_info = (NPatchInfo) {(Rectangle) {88, 1, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};

    DrawTextureNPatch(atlas, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    patch_info = (NPatchInfo) {(Rectangle) {88, 11, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};

    DrawTextureNPatch(atlas, patch_info, inflate_rectangle((Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width * progress_bar.progress,
            .height = control.rectangle.height,
    }, -1), (Vector2) {0}, 0.0f, WHITE);
}

void draw_node(t_control control, t_node node, size_t index, size_t subdepth) {
    float x = control.rectangle.x + (10 * subdepth);
    float y = control.rectangle.y + (10 * index);
    DrawTextEx(font, node.text, (Vector2) {x, y}, font.baseSize, 0.0f, control.is_enabled ? BLACK : (Color) {109, 109, 109, 255});
}

void walk_tree(t_control control, const t_node *node, size_t accumulator, size_t subdepth) {
    printf("[%d, %d] %s\n", accumulator, subdepth, node->text);

    subdepth++;
    draw_node(control, *node, accumulator, subdepth);

    for (size_t i = 0; i < node->children_length; i++) {
        walk_tree(control, &node->children[i], ++accumulator, subdepth);
    }
}

void raylib_draw_treeview(t_control control, e_visual_state visual_state, t_treeview treeview) {

    raylib_draw_listbox(control, visual_state, (t_listbox) {0});

    walk_tree(control, &treeview.root_node, 0, 0);


}
