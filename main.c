#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include "raylib.h"
#include "c_ugui.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

#define WINDOW_TITLE "Window title"

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

    Vector2 text_bounds = MeasureTextEx(font, button.text, 14.0f, 0.0f);

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
            thumb_src_rectangle = (Rectangle) {53, 33, 19, 11};
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

const float listbox_item_height = 16.0f;

void raylib_draw_listbox(t_control control, e_visual_state visual_state, t_listbox listbox) {
    const float padding = 2.0f;
    NPatchInfo patch_info = (NPatchInfo) {(Rectangle) {36, 6, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};

    DrawTextureNPatch(atlas, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y,
            .width = control.rectangle.width,
            .height = control.rectangle.height,
    }, (Vector2) {0}, 0.0f, WHITE);

    float list_height = listbox_item_height * listbox.items_length;

    size_t index_begin = (listbox.translation * (list_height - control.rectangle.height)) / listbox_item_height;
    size_t index_end = ((listbox.translation * list_height) + control.rectangle.height) / listbox_item_height;

    index_begin = imax2(index_begin, 0);
    index_end = imin2(index_end, listbox.items_length);

    BeginScissorMode(control.rectangle.x, control.rectangle.y, control.rectangle.width, control.rectangle.height);
    for (int i = 0; i < listbox.items_length; ++i) {

        float y = (listbox_item_height * i) -
                  (listbox.translation * ((listbox_item_height * listbox.items_length) - control.rectangle.height));

        if (listbox.selected_index == i) {
            NPatchInfo patch_info = (NPatchInfo) {(Rectangle) {34, 11, 5, 5}, 2, 2, 2, 2, NPATCH_NINE_PATCH};

            DrawTextureNPatch(atlas, patch_info, (Rectangle) {
                    .x = control.rectangle.x,
                    .y = control.rectangle.y + y,
                    .width = control.rectangle.width,
                    .height = listbox_item_height,
            }, (Vector2) {0}, 0.0f, WHITE);
        }

        DrawTextEx(font, listbox.items[i], (Vector2) {
                control.rectangle.x + padding,
                control.rectangle.y + y,
        }, font.baseSize, 0.0f, listbox.selected_index == i ? WHITE : BLACK);

    }
    EndScissorMode();
}

size_t raylib_get_listbox_index_from_y(t_control control, t_listbox listbox, float mouse_y) {
    size_t i = floor(((mouse_y - control.rectangle.y) + (listbox.translation *
                                                         ((listbox_item_height * listbox.items_length) -
                                                          control.rectangle.height))) /
                     listbox_item_height);
    return i;
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    t_renderer renderer = (t_renderer) {
            .draw_button = raylib_draw_button,
            .draw_togglebutton = raylib_draw_togglebutton,
            .draw_textbox = raylib_draw_textbox,
            .draw_slider = raylib_draw_slider,
            .draw_listbox = raylib_draw_listbox,
            .measure_text = raylib_measure_text,
            .get_listbox_index_from_y = raylib_get_listbox_index_from_y,
    };

    atlas = LoadTexture("assets/windows-10.png");
    font = LoadFontEx("assets/micross.ttf", 14, NULL, 250);

    int32_t is_checked = 1;
    float value = 0.5f;
    t_textbox textbox = (t_textbox) {
            .text = (char *) calloc(9000, sizeof(char)),
            .text_length = 9000,
            .selection_start_index = 0,
            .selection_end_index = 0,
            .caret_index = 0,
    };
    const char *strings[] = {"one", "two", "three", "one", "two", "three", "one", "two", "three", "one", "two", "three",
                             "one", "two", "three"};
    t_listbox listbox = (t_listbox) {
            .items = strings,
            .items_length = sizeof(strings) / sizeof(strings[0]),
            .selected_index = 0,
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);


        int32_t pressed_chars[255] = {0};
        int32_t pressed_chars_length = 0;
        while (true) {
            pressed_chars[pressed_chars_length] = GetCharPressed();
            if (!pressed_chars[pressed_chars_length]) break;
            pressed_chars_length++;
        }

        int32_t pressed_keycodes[255] = {0};
        int32_t pressed_keycodes_length = 0;
        while (true) {
            pressed_keycodes[pressed_keycodes_length] = GetKeyPressed();
            if (!pressed_keycodes[pressed_keycodes_length]) break;
            pressed_keycodes_length++;
        }

        gui_begin_frame((t_input) {
                .mouse_position = (t_vector2) {.x = GetMousePosition().x, .y = GetMousePosition().y},
                .is_primary_mouse_button_down = IsMouseButtonDown(0),
                .pressed_chars = pressed_chars,
                .pressed_chars_length = pressed_chars_length,
                .pressed_keycodes = pressed_keycodes,
                .pressed_keycodes_length = pressed_keycodes_length,
                .mouse_wheel_delta = (t_vector2) {.x = GetMouseWheelMoveV().x, .y = GetMouseWheelMoveV().y / 2.0f},
        }, &renderer);

        if (gui_button((t_control) {
                .uid = 0,
                .rectangle = (t_rectangle) {
                        .x = 10,
                        .y = 10,
                        . width = 90,
                        .height = 23
                },
                .is_enabled = 1
        }, (t_button) {
                .text = "Ok"
        })) {
            SetWindowSize(GetScreenWidth() + 5, GetScreenHeight() + 5);
        }

        is_checked = gui_togglebutton((t_control) {
                .uid = 1,
                .rectangle = (t_rectangle) {
                        .x = 10,
                        .y = 40,
                        . width = 90,
                        .height = 23
                },
                .is_enabled = 1
        }, (t_togglebutton) {
                .text = "abc",
                .is_checked = is_checked
        });

        textbox = gui_textbox((t_control) {
                .uid = 2,
                .rectangle = (t_rectangle) {
                        .x = 10,
                        .y = 80,
                        . width = 90,
                        .height = 23
                },
                .is_enabled = 1
        }, textbox);

        value = gui_slider((t_control) {
                .uid = 3,
                .rectangle = (t_rectangle) {
                        .x = 10,
                        .y = 120,
                        .width = 40,
                        .height = 90
                },
                .is_enabled = 1
        }, (t_slider) {
                .value = value
        });

        value = gui_slider((t_control) {
                .uid = 4,
                .rectangle = (t_rectangle) {
                        .x = 100,
                        .y = 120,
                        .width = 90,
                        .height = 40
                },
                .is_enabled = 1
        }, (t_slider) {
                .value = value
        });

        listbox = gui_listbox((t_control) {
                .uid = 5,
                .rectangle = (t_rectangle) {
                        .x = 300,
                        .y = 120,
                        .width = 120,
                        .height = 300
                },
                .is_enabled = 1
        }, listbox);

        EndDrawing();
    }

    UnloadTexture(atlas);

    CloseWindow();

    return 0;
}
