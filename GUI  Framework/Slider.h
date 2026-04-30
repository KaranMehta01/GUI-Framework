#ifndef SLIDER_H
#define SLIDER_H

#include "Widget.h"
#include "RenderingFunctions.h"
#include <functional>
#include <algorithm>

class Slider : public Widget{
private:
    float min_value;
    float max_value;
    float current_value;
    bool is_dragging;
    float knob_slider_ratio;

    std::function<void(float)> on_value_changed;

public:
    Slider(Rect bounds, float min_val, float max_val, float initial_val, float knob_slider_ratio = 0.1)
        : Widget(bounds), min_value(min_val), max_value(max_val), 
        current_value(initial_val), is_dragging(false){
        this->knob_slider_ratio = std::min(0.5f, std::max(0.1f, knob_slider_ratio));
    }

    void set_on_value_changed(std::function<void(float)> callback){
        on_value_changed = callback;
    }

    float get_value() const {return current_value;}

    void set_value(float new_value) {
        current_value = std::max(min_value, std::min(new_value, max_value));
        
        if (on_value_changed) {
            on_value_changed(current_value);
        }
    }

    void render(GUIContext& ctx) override {
        if (!is_visible) return;

        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, ctx.theme.SECONDARY_COLOR);

        float percentage = (current_value - min_value) / (max_value - min_value);
        float slider_width = static_cast<float>(absolute_bounds.x2) - float(absolute_bounds.x1) + 1.0f;
        float knob_width = slider_width * knob_slider_ratio;
        uint32_t knob_x1 = absolute_bounds.x1 + static_cast<uint32_t>(percentage * (slider_width - knob_width));

        uint32_t knob_color = ((state == WidgetState::HOVERED) || is_dragging) ? ctx.theme.PRIMARY_COLOR : ctx.theme.FOREGROUND_COLOR;

        draw_rectangle(
            ctx,
            knob_x1, absolute_bounds.y1,
            knob_x1 + knob_width, absolute_bounds.y2,
            knob_color
        );
    }

    Widget* handle_event(const GUIEvent& event) override {
        Widget* child_handled = Widget::handle_event(event);
        if (child_handled != nullptr) return child_handled;

        if (event.type == GUIEventType::MOUSE_MOVE){
            if (is_dragging){
                float width = static_cast<float>(absolute_bounds.x2 - absolute_bounds.x1 + 1);
                float relative_x = static_cast<float>(event.mouse_x) - static_cast<float>(absolute_bounds.x1);

                float percentage = std::max(0.0f, std::min(1.0f, relative_x / width));
                current_value = min_value + percentage * (max_value - min_value);

                if (on_value_changed) on_value_changed(current_value);
                return this;
            }

            if (absolute_bounds.contains(event.mouse_x, event.mouse_y)){
                state = WidgetState::HOVERED;
                return this;
            }
            else{
                state = WidgetState::NORMAL;
            }
        }
        else if ((event.type == GUIEventType::MOUSE_BUTTON_DOWN) && event.is_left_button){
            if (absolute_bounds.contains(event.mouse_x, event.mouse_y)){
                is_dragging = true;
                state = WidgetState::PRESSED;

                handle_event(GUIEvent{GUIEventType::MOUSE_MOVE, event.mouse_x, event.mouse_y, false, GUIKeyCode{}, 0});

                return this;
            }
        }
        else if ((event.type == GUIEventType::MOUSE_BUTTON_UP) && event.is_left_button){
            if (is_dragging){
                is_dragging = false;
                state = absolute_bounds.contains(event.mouse_x, event.mouse_y) ? WidgetState::HOVERED : WidgetState::NORMAL;

                return this;
            }
        }

        return nullptr;
    }
};

#endif