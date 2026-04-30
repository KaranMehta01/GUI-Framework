#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <functional>
#include "Widget.h"
#include "RenderingFunctions.h"
#include "Glyphs7X7.h"
#include "Glyphs16X16.h"

class Button : public Widget{
private:
    std::string text;
    bool is_clicking;
    std::function<void()> on_click;

    bool high_res_text;

    float horizontal_padding;
    float vertical_padding;

public:
    Button(Rect bounds, const std::string& button_text = "", bool render_high_res_text = false, float horizontal_padding = 0.025f, float vertical_padding = 0.1f)
        : Widget(bounds), text(button_text), is_clicking(false), on_click(nullptr), high_res_text(render_high_res_text){
            this->horizontal_padding = std::min(0.25f, std::max(0.0f, horizontal_padding));
            this->vertical_padding = std::min(0.25f, std::max(0.0f, vertical_padding));
    }

    void set_on_click(std::function<void()> callback){
        on_click = callback;
    }

    void set_text(const std::string& new_text){
        text = new_text;
    }

    void render(GUIContext& ctx){
        if (!is_visible) return;

        Rect old_clip = ctx.clip_rect;
        ctx.clip_rect = ctx.clip_rect.intersection(absolute_bounds);

        if ((ctx.clip_rect.x1 >= ctx.clip_rect.x2) || (ctx.clip_rect.y1 >= ctx.clip_rect.y2)){
            ctx.clip_rect = old_clip;
            return;
        }

        uint32_t body_color;
        switch (state){
            case WidgetState::PRESSED:
                body_color = ctx.theme.SECONDARY_COLOR;
                break;
            case WidgetState::HOVERED:
                body_color = blend_color(ctx.theme.PRIMARY_COLOR, 0xFFFFFFFF, 0.2);
                break;
            case WidgetState::NORMAL:
            default:
                body_color = ctx.theme.PRIMARY_COLOR;
                break;
        }

        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, body_color);

        // Text rendering
        if (!text.empty()){
            float text_spacing_to_text_width_ratio = 0.125;

            float text_width  = static_cast<float>(absolute_bounds.x2 - absolute_bounds.x1 + 1) * horizontal_padding;
            float text_height = static_cast<float>(absolute_bounds.y2 - absolute_bounds.y1 + 1) * vertical_padding;

            uint32_t text_x1 = static_cast<uint32_t>(absolute_bounds.x1 + text_width);
            uint32_t text_y1 = static_cast<uint32_t>(absolute_bounds.y1 + text_height);
            uint32_t text_x2 = static_cast<uint32_t>(absolute_bounds.x2 - text_width);
            uint32_t text_y2 = static_cast<uint32_t>(absolute_bounds.y2 - text_height);

            float no_of_characters = text.size();

            float char_size = static_cast<float>(text_x2 - text_x1 + 1) / (no_of_characters + text_spacing_to_text_width_ratio * (no_of_characters - 1.0));
            float char_spacing = char_size * text_spacing_to_text_width_ratio;

            float cursor_x = static_cast<float>(text_x1);

            if (high_res_text){
                for (char c : text) {
                    const bool (&glyph)[16][16] = get_glyph_16x16(c);
                    
                    draw_high_res_glyph(ctx, cursor_x, text_y1, cursor_x + char_size - 1, text_y2, glyph, ctx.theme.FOREGROUND_COLOR);
                    
                    cursor_x += (char_size + char_spacing);
                }
            }
            else{
                for (char c : text) {
                    const bool (&glyph)[7][7] = get_glyph_7x7(c);
                    
                    draw_glyph(ctx, cursor_x, text_y1, cursor_x + char_size - 1, text_y2, glyph, ctx.theme.FOREGROUND_COLOR);
                    
                    cursor_x += (char_size + char_spacing);
                }
            }
        }

        ctx.clip_rect = old_clip;
    }

    Widget* handle_event(const GUIEvent& event) override {
        Widget* child_handeled = Widget::handle_event(event);
        if (child_handeled != nullptr) return child_handeled;

        bool is_inside = absolute_bounds.contains(event.mouse_x, event.mouse_y);

        if (event.type == GUIEventType::MOUSE_MOVE){
            if (is_clicking){
                state = is_inside ? WidgetState::PRESSED : WidgetState::NORMAL;
                return this;
            }

            if (is_inside){
                state = WidgetState::HOVERED;
                return this;
            }
            else{
                state = WidgetState::NORMAL;
            }
        }
        else if ((event.type == GUIEventType::MOUSE_BUTTON_DOWN) && event.is_left_button){
            if (is_inside){
                is_clicking = true;
                state = WidgetState::PRESSED;
                return this;
            }
        }
        else if ((event.type == GUIEventType::MOUSE_BUTTON_UP) && event.is_left_button){
            if (is_clicking){
                is_clicking = false;
                state = is_inside ? WidgetState::HOVERED : WidgetState::NORMAL;

                if (is_inside && on_click){
                    on_click();
                }
                return this;
            }
        }

        return nullptr;
    }
};

#endif