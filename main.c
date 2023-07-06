#include <malloc.h>
#include <stdio.h>
#include "c_ugui.h"
#include "c_ugui_raylib_renderer.h"

#define SCREEN_WIDTH (800)
#define SCREEN_HEIGHT (450)

#define WINDOW_TITLE "Window title"

t_visual_style get_windows_10_visual_style() {
    t_visual_style visual_style = (t_visual_style) {0};

    visual_style.texture = LoadTexture("assets/windows-10.png");
    visual_style.font = LoadFontEx("assets/micross.ttf", 14, 0, 250);

    visual_style.raised_frames[0] = (NPatchInfo) {(Rectangle) {1, 1, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    visual_style.raised_frames[1] = (NPatchInfo) {(Rectangle) {1, 12, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    visual_style.raised_frames[2] = (NPatchInfo) {(Rectangle) {1, 23, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    visual_style.raised_frames[3] = (NPatchInfo) {(Rectangle) {1, 34, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};

    visual_style.list_frames[0] = (NPatchInfo) {(Rectangle) {36, 6, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};
    visual_style.list_frames[1] = (NPatchInfo) {(Rectangle) {36, 6, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};
    visual_style.list_frames[2] = (NPatchInfo) {(Rectangle) {36, 6, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};
    visual_style.list_frames[3] = (NPatchInfo) {(Rectangle) {36, 6, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};

    visual_style.list_selected_frames[0] = (NPatchInfo) {(Rectangle) {34, 11, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    visual_style.list_selected_frames[1] = (NPatchInfo) {(Rectangle) {34, 11, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    visual_style.list_selected_frames[2] = (NPatchInfo) {(Rectangle) {34, 11, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    visual_style.list_selected_frames[3] = (NPatchInfo) {(Rectangle) {34, 11, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};

    visual_style.edit_frames[0] = (NPatchInfo) {(Rectangle) {20, 1, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    visual_style.edit_frames[1] = (NPatchInfo) {(Rectangle) {20, 6, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    visual_style.edit_frames[2] = (NPatchInfo) {(Rectangle) {20, 11, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};
    visual_style.edit_frames[3] = (NPatchInfo) {(Rectangle) {20, 16, 5, 5}, 3, 3, 3, 3, NPATCH_NINE_PATCH};

    visual_style.slider_tracks[0] = (NPatchInfo) {(Rectangle) {36, 1, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};
    visual_style.slider_tracks[1] = (NPatchInfo) {(Rectangle) {36, 1, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};
    visual_style.slider_tracks[2] = (NPatchInfo) {(Rectangle) {36, 1, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};
    visual_style.slider_tracks[3] = (NPatchInfo) {(Rectangle) {36, 1, 3, 3}, 2, 2, 2, 2, NPATCH_NINE_PATCH};

    visual_style.slider_horizontal_thumbs[0] = (Rectangle) {40, 0, 11, 19};
    visual_style.slider_horizontal_thumbs[1] = (Rectangle) {40, 19, 11, 19};
    visual_style.slider_horizontal_thumbs[2] = (Rectangle) {40, 38, 11, 19};
    visual_style.slider_horizontal_thumbs[3] = (Rectangle) {40, 76, 11, 19};

    visual_style.slider_vertical_thumbs[0] = (Rectangle) {53, 0, 19, 11};
    visual_style.slider_vertical_thumbs[1] = (Rectangle) {53, 11, 19, 11};
    visual_style.slider_vertical_thumbs[2] = (Rectangle) {53, 22, 19, 11};
    visual_style.slider_vertical_thumbs[3] = (Rectangle) {53, 44, 19, 11};

    visual_style.progressbar_bars[0] = (NPatchInfo) {(Rectangle) {88, 11, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    visual_style.progressbar_bars[1] = (NPatchInfo) {(Rectangle) {88, 11, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    visual_style.progressbar_bars[2] = (NPatchInfo) {(Rectangle) {88, 11, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};
    visual_style.progressbar_bars[3] = (NPatchInfo) {(Rectangle) {88, 11, 11, 9}, 6, 5, 6, 5, NPATCH_NINE_PATCH};

    visual_style.list_content_padding = (Vector2) {4, 0};
    return visual_style;
}

t_visual_style get_windows_11_visual_style() {

    t_visual_style visual_style = get_windows_10_visual_style();

    UnloadTexture(visual_style.texture);

    visual_style.texture = LoadTexture("assets/windows-11.png");

    return visual_style;
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
            .listbox_get_item_height = raylib_listbox_get_item_height,
            .draw_progressbar = raylib_draw_progressbar,
            .draw_treeview = raylib_draw_treeview,
            .draw_treeview_node = raylib_draw_treeview_node
    };

    raylib_set_visual_style(get_windows_10_visual_style());

    int32_t is_checked = 1;
    float some_range_value = 0.5f;
    t_textbox textbox = (t_textbox) {
            .text = (char *) calloc(9000, sizeof(char)),
            .text_length = 9000,
            .selection_start_index = 0,
            .selection_end_index = 0,
            .caret_index = 0,
    };
    char *strings[50];
    for (int i = 0; i < sizeof(strings) / sizeof(strings[0]); ++i) {
        char *str = (char *) calloc(260, sizeof(char));
        sprintf(str, "%d", i);
        strings[i] = str;
    }
    t_listbox listbox = (t_listbox) {
            .items = strings,
            .items_length = sizeof(strings) / sizeof(strings[0]),
            .selected_index = 0,
    };
    t_treeview treeview = (t_treeview) {
            .root_node = (t_node) {
                    .text = "Root",
                    .children_length = 3,
                    .is_expanded = 1,
                    .children = (t_node[]) {
                            (t_node) {
                                    .text = "Root 1",
                                    .is_expanded = 1,
                            },
                            (t_node) {
                                    .text = "Root 2",
                                    .is_expanded = 1,
                            },
                            (t_node) {
                                    .text = "I'm an alcoholic",
                                    .children_length = 2,
                                    .is_expanded = 1,
                                    .children = (t_node[]) {
                                            (t_node) {
                                                    .text = "I'm an alcoholic 2",
                                                    .is_expanded = 1,
                                            },
                                            (t_node) {
                                                    .text = "I'm an alcoholic 3",
                                                    .is_expanded = 1,
                                            },
                                    }
                            }
                    }
            }
    };

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color) {240, 240, 240, 255});


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
                .mouse_wheel_delta = (t_vector2) {.x = GetMouseWheelMoveV().x, .y = GetMouseWheelMoveV().y},
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
                .text = "Windows 10"
        })) {
            raylib_unload_visual_style();
            raylib_set_visual_style(get_windows_10_visual_style());
        }

        if (gui_button((t_control) {
                .uid = 1,
                .rectangle = (t_rectangle) {
                        .x = 120,
                        .y = 10,
                        . width = 90,
                        .height = 23
                },
                .is_enabled = 1
        }, (t_button) {
                .text = "Windows 11"
        })) {
            raylib_unload_visual_style();
            raylib_set_visual_style(get_windows_11_visual_style());
        }

        is_checked = gui_togglebutton((t_control) {
                .uid = 2,
                .rectangle = (t_rectangle) {
                        .x = 10,
                        .y = 40,
                        . width = 90,
                        .height = 23
                },
                .is_enabled = 1
        }, (t_togglebutton) {
                .text = "enabled",
                .is_checked = is_checked
        });

        textbox = gui_textbox((t_control) {
                .uid = 3,
                .rectangle = (t_rectangle) {
                        .x = 10,
                        .y = 80,
                        . width = 90,
                        .height = 23
                },
                .is_enabled = is_checked
        }, textbox);

        some_range_value = gui_slider((t_control) {
                .uid = 4,
                .rectangle = (t_rectangle) {
                        .x = 10,
                        .y = 120,
                        .width = 40,
                        .height = 90
                },
                .is_enabled = is_checked
        }, (t_slider) {
                .value = some_range_value
        });

        some_range_value = gui_slider((t_control) {
                .uid = 5,
                .rectangle = (t_rectangle) {
                        .x = 100,
                        .y = 120,
                        .width = 90,
                        .height = 40
                },
                .is_enabled = is_checked
        }, (t_slider) {
                .value = some_range_value
        });

        listbox = gui_listbox((t_control) {
                .uid = 6,
                .rectangle = (t_rectangle) {
                        .x = 300,
                        .y = 120,
                        .width = 120,
                        .height = 300
                },
                .is_enabled = is_checked
        }, listbox);
        gui_progresbar((t_control) {
                .uid = 7,
                .rectangle = (t_rectangle) {
                        .x = 150,
                        .y = 300,
                        .width = 120,
                        .height = 25
                },
                .is_enabled = is_checked
        }, (t_progressbar) {
                .progress = some_range_value
        });

        treeview = gui_treeview((t_control) {
                .uid = 8,
                .rectangle = (t_rectangle) {
                        .x = 450,
                        .y = 120,
                        .width = 250,
                        .height = 300
                },
                .is_enabled = is_checked
        }, treeview);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
