//
// Created by https://github.com/Aurumaker72/
//

#include <string.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include "c_ugui.h"

t_vector2 primary_mouse_down_position = {0};
t_input input = {0}, last_input = {0};
t_renderer *renderer;
int32_t focus_uid = -1;


void gui_begin_frame(t_input _input, t_renderer *_renderer) {
    last_input = input;
    input = _input;
    renderer = _renderer;
    if (input.is_primary_mouse_button_down && !last_input.is_primary_mouse_button_down) {
        primary_mouse_down_position = input.mouse_position;
    }
}

e_visual_state get_visual_state(t_control control) {
    if (!control.is_enabled) return e_visual_state_disabled;

    if (is_vector2_inside(input.mouse_position, control.rectangle)) {
        if (input.is_primary_mouse_button_down && is_vector2_inside(primary_mouse_down_position, control.rectangle)) {
            return e_visual_state_active;
        }
        return e_visual_state_hovered;
    }

    return e_visual_state_normal;
}

void strslice(const char *str, char *result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
}

void strremoveregion(char *str, size_t start, size_t end) {
    size_t length = strlen(str);

    if (start >= end || end >= length)
        return;

    memmove(str + start, str + end + 1, length - end);
    str[length - (end - start + 1)] = '\0';
}

void strinsert(char *dst, const char *src, size_t index) {
    size_t src_length = strlen(src);
    memmove(dst + index + src_length, dst + index, strlen(dst) - index + 1);
    memcpy(dst + index, src, src_length);
}

void strremch(char *str, size_t index) {
    memmove(&str[index], &str[index + 1], strlen(str) - index);
}

void strremslice(char *str, size_t from, size_t to) {
    size_t len = strlen(str);

    if (from >= len || from >= to || to > len) {
        return;
    }

    memmove(str + from, str + to, len - to + 1);
}

float fclamp(float value, float min, float max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

int32_t iclamp(int32_t value, int32_t min, int32_t max) {
    if (value > max) return max;
    if (value < min) return min;
    return value;
}

int32_t imin(int32_t a, int32_t b) {
    if (a > b) return b;
    return a;
}

int32_t imax(int32_t a, int32_t b) {
    if (a > b) return a;
    return b;
}

int32_t get_caret_index_for_relative_position(const char *text, float target) {
    int32_t matching_caret_index = 0;
    float lowest_distance = 9999999.0f;

    for (int i = 0; i < strlen(text) + 1; ++i) {
        char *sliced_str = (char *) calloc(strlen(text), sizeof(char));

        strslice(text, sliced_str, 0, i);

        float x = renderer->measure_text(sliced_str).x;

        if (fabsf(target - x) < lowest_distance) {
            matching_caret_index = i;
            lowest_distance = fabsf(target - x);
        }

        free(sliced_str);

    }
    return matching_caret_index;
}

int32_t is_primary_interacting(t_control control) {
    return (input.is_primary_mouse_button_down && !last_input.is_primary_mouse_button_down) &&
           is_vector2_inside(primary_mouse_down_position, control
                   .rectangle) && control.is_enabled;
}


int32_t gui_get_focus_uid() {
    return focus_uid;
}

int32_t gui_button(t_control control, t_button button) {

    if (is_primary_interacting(control)) {
        focus_uid = control.uid;
        return 1;
    }
    renderer->draw_button(control, get_visual_state(control), button);

    return 0;
}

int32_t gui_togglebutton(t_control control, t_togglebutton togglebutton) {
    e_visual_state visual_state = get_visual_state(control);
    if (togglebutton.is_checked && control.is_enabled) {
        visual_state = e_visual_state_active;
    }
    if (is_primary_interacting(control)) {
        togglebutton.is_checked ^= 1;
        focus_uid = control.uid;
    }
    renderer->draw_togglebutton(control, visual_state, togglebutton);

    return togglebutton.is_checked;
}


t_textbox gui_textbox(t_control control, t_textbox textbox) {
    e_visual_state visual_state = get_visual_state(control);
    // if we capture the focus, the control has to remain in the active visual state (emulates commctl behaviour)
    if (focus_uid == control.uid) {
        visual_state = e_visual_state_active;
    }
    renderer->draw_textbox(control, visual_state, textbox);

    // after this point, only editing and selection operations are performed
    // we can return early here if the control is disabled
    if (!control.is_enabled)
        return textbox;

    if (is_primary_interacting(control)) {
        focus_uid = control.uid;

        // we need to find a caret index from the mouse's X position
        float target = input.mouse_position.x - control.rectangle.x;
        int32_t caret_index = get_caret_index_for_relative_position(textbox.text, target);
        textbox.caret_index = textbox.selection_start_index = textbox.selection_end_index = caret_index;
    }

    // extending the selection after selection start was initially created
    if (is_vector2_inside(primary_mouse_down_position, control.rectangle) && input.is_primary_mouse_button_down) {
        float target = input.mouse_position.x - control.rectangle.x;
        textbox.selection_end_index = get_caret_index_for_relative_position(textbox.text, target);
    }


    // insert all pressed chars
    for (int i = 0; i < input.pressed_chars_length; ++i) {
        // if we dont have a selection, replace the selected text with the new char, otherwise just insert it at the caret
        if (textbox.selection_start_index == textbox.selection_end_index) {
            strinsert(textbox.text, (const char[]) {input.pressed_chars[i], '\0'}, textbox.caret_index);
            textbox.caret_index++;
        } else {
            int32_t lower_index = imin(textbox.selection_start_index, textbox.selection_end_index);
            int32_t higher_index = imax(textbox.selection_start_index, textbox.selection_end_index);

            strremslice(textbox.text, lower_index, higher_index);
            strinsert(textbox.text, (const char[]) {input.pressed_chars[i], '\0'}, lower_index);
            textbox.caret_index = textbox.selection_start_index = textbox.selection_end_index = higher_index;
        }

    }

    // process special inputs
    for (int i = 0; i < input.pressed_keycodes_length; ++i) {
        if (input.pressed_keycodes[i] == e_keycodes_backspace) {
            // either remove one character behind the caret, or lift out the entire selection
            if (textbox.selection_start_index == textbox.selection_end_index) {

                // caret-deletion at index 0 is not possible
                if (textbox.caret_index > 0) {
                    strremch(textbox.text, textbox.caret_index - 1);
                    textbox.caret_index--;
                }

            } else {
                int32_t lower_index = imin(textbox.selection_start_index, textbox.selection_end_index);
                int32_t higher_index = imax(textbox.selection_start_index, textbox.selection_end_index);

                strremslice(textbox.text, lower_index, higher_index);
                textbox.caret_index = textbox.selection_start_index = textbox.selection_end_index = lower_index;
            }
        } else if (input.pressed_keycodes[i] == e_keycodes_left) {
            // if there's a selection, we nuke it and put the caret at the selection's lower index
            if (textbox.selection_start_index == textbox.selection_end_index) {
                textbox.caret_index--;
            } else {
                textbox.caret_index = imin(textbox.selection_start_index, textbox.selection_end_index);
                textbox.selection_start_index = textbox.selection_end_index = textbox.caret_index;
            }
        } else if (input.pressed_keycodes[i] == e_keycodes_right) {
            // if there's a selection, we nuke it and put the caret at the selection's higher index
            if (textbox.selection_start_index == textbox.selection_end_index) {
                textbox.caret_index++;
            } else {
                textbox.caret_index = imax(textbox.selection_start_index, textbox.selection_end_index);
                textbox.selection_start_index = textbox.selection_end_index = textbox.caret_index;
            }
        }
    }

    textbox.caret_index = iclamp(textbox.caret_index, 0, strlen(textbox.text));

    return textbox;
}

float gui_slider(t_control control, t_slider slider) {
    e_visual_state visual_state = get_visual_state(control);
    // if we capture the focus, the control has to remain in the active visual state (emulates commctl behaviour)
    if (focus_uid == control.uid) {
        visual_state = e_visual_state_active;
    }

    if (focus_uid == control.uid && !input.is_primary_mouse_button_down) {
        focus_uid = -1;
    }

    if (is_primary_interacting(control)) {
        focus_uid = control.uid;
    }

    if (focus_uid == control.uid) {
        // if width>height, we have a horizontal slider, otherwise a vertical one
        if (control.rectangle.width > control.rectangle.height) {
            slider.value = (input.mouse_position.x - control.rectangle.x) / control.rectangle.width;
        } else {
            slider.value = (input.mouse_position.y - control.rectangle.y) / control.rectangle.height;
        }
        slider.value = fclamp(slider.value, 0.0f, 1.0f);
    }
    renderer->draw_slider(control, visual_state, slider);

    return slider.value;
}

t_listbox gui_listbox(t_control control, t_listbox listbox) {
    if (focus_uid == control.uid && !input.is_primary_mouse_button_down) {
        focus_uid = -1;
    }
    if (is_primary_interacting(control)) {
        focus_uid = control.uid;
    }
    if (focus_uid == control.uid) {
        // we clamp to one below length, since length is not 0-indexed
        listbox.selected_index = iclamp(
                renderer->get_listbox_index_from_y(control, listbox, input.mouse_position.y), 0,
                listbox.items_length - 1);

    }

    if (control.is_enabled && is_vector2_inside(input.mouse_position, control.rectangle) &&
        input.mouse_wheel_delta.y != 0) {
        float sign = input.mouse_wheel_delta.y > 0.0f ? -1.0f : 1.0f;

        listbox.translation += (sign != 0.0f ? (20.0f / (20.0f * listbox.items_length)) : 0) * sign;
    }

    listbox.translation = fclamp(listbox.translation, 0.0f, 1.0f);

    renderer->draw_listbox(control, e_visual_state_normal, listbox);

    return listbox;
}
