#include <string.h>
#include <malloc.h>
#include <stdio.h>
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
    }, 14.0f, 0.0f, control.is_enabled ? BLACK : (Color) {
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
    }, 14.0f, 0.0f, control.is_enabled ? BLACK : (Color) {
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
    }, 14.0f, 0.0f, WHITE);
    EndScissorMode();
}

void raylib_draw_slider(t_control control, e_visual_state visual_state, t_slider slider) {
    const float track_thickness = 4.0f;
    const float thumb_width = 11.0f; // dimensions are 11x9, but for some reason raylib misses the last row of pixels
    const float thumb_height = 19.0f;
    NPatchInfo patch_info = (NPatchInfo) {(Rectangle) {36, 1, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};


    DrawTextureNPatch(atlas, patch_info, (Rectangle) {
            .x = control.rectangle.x,
            .y = control.rectangle.y + control.rectangle.height / 2 - track_thickness / 2,
            .width = control.rectangle.width,
            .height = track_thickness,
    }, (Vector2) {0}, 0.0f, WHITE);


    Rectangle thumb_rectangle;

    if (visual_state == e_visual_state_normal) {
        thumb_rectangle = (Rectangle) {40, 0, 11, 19};
    } else if (visual_state == e_visual_state_hovered) {
        thumb_rectangle = (Rectangle) {40, 19, 11, 19};
    } else if (visual_state == e_visual_state_active) {
        thumb_rectangle = (Rectangle) {40, 38, 11, 19};
    } else {
        thumb_rectangle = (Rectangle) {40, 76, 11, 19};
    }

    DrawTexturePro(atlas, thumb_rectangle, (Rectangle) {
            control.rectangle.x + (control.rectangle.width * slider.value) - thumb_width / 2,
            control.rectangle.y + control.rectangle.height / 2 - thumb_height / 2,
            thumb_width,
            thumb_height
    }, (Vector2) {0}, 0.0f, WHITE);
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    t_renderer renderer = (t_renderer) {
            .draw_button = raylib_draw_button,
            .draw_togglebutton = raylib_draw_togglebutton,
            .draw_textbox = raylib_draw_textbox,
            .draw_slider = raylib_draw_slider,
            .measure_text = raylib_measure_text,
    };

    atlas = LoadTexture("assets/windows-11.png");
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

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mouse_position = GetMousePosition();

        int32_t pressed_chars[255] = {0};
        int32_t i = 0;
        while (true) {
            pressed_chars[i] = GetCharPressed();
            if (!pressed_chars[i]) break;
            i++;
        }

        int32_t pressed_keycodes[255] = {0};
        int32_t j = 0;
        while (true) {
            pressed_keycodes[i] = GetKeyPressed();

            if (!pressed_keycodes[i]) break;
            printf("%d\n", pressed_keycodes[i]);
            j++;
        }

        gui_begin_frame((t_input) {
                .mouse_position = (t_vector2) {.x = mouse_position.x, .y = mouse_position.y},
                .is_primary_mouse_button_down = IsMouseButtonDown(0),
                .pressed_chars = pressed_chars,
                .pressed_chars_length = i,
                .pressed_keycodes = pressed_keycodes,
                .pressed_keycodes_length = j,
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
                        . width = 90,
                        .height = 23
                },
                .is_enabled = 1
        }, (t_slider) {
                .value = value
        });

        EndDrawing();
    }

    UnloadTexture(atlas);

    CloseWindow();

    return 0;
}
