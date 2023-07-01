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
    renderer->draw_button(control, get_visual_state(control), button);

    if (is_primary_interacting(control)) {
        focus_uid = control.uid;
        return 1;
    }

    return 0;
}

int32_t gui_togglebutton(t_control control, t_togglebutton togglebutton) {
    e_visual_state visual_state = get_visual_state(control);
    if (togglebutton.is_checked && control.is_enabled) {
        visual_state = e_visual_state_active;
    }
    renderer->draw_togglebutton(control, visual_state, togglebutton);

    if (is_primary_interacting(control)) {
        togglebutton.is_checked ^= 1;
        focus_uid = control.uid;
    }
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
        return;

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


    // insert all pressed chars at the caret index
    for (int i = 0; i < input.pressed_chars_length; ++i) {
        strinsert(textbox.text, (const char[]) {input.pressed_chars[i], '\0'}, textbox.caret_index);
        textbox.caret_index++;
    }

    // process special inputs
    for (int i = 0; i < input.pressed_keycodes_length; ++i) {
        printf("%d\n", input.pressed_keycodes[i]);
        if (input.pressed_keycodes[i] == e_keycodes_backspace) {
            // either remove one character behind the caret, or the selection

            // TODO: implement this
            if (textbox.selection_start_index == textbox.selection_end_index) {

            } else {

            }
        }
    }

    return textbox;
}
