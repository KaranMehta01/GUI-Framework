#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include <string>
#include "Widget.h"
#include "RenderingFunctions.h"
#include "Glyphs7X7.h"
#include "Glyphs16X16.h"

class TextBox : public Widget{
private:
    std::string text;
    bool high_res_text;
    float horizontal_padding;
    float vertical_padding;
    
    float blink_timer;
    bool show_cursor;

public:
    TextBox(Rect bounds, const std::string& initial_text = "", bool render_high_res_text = false, float h_pad = 0.025f, float v_pad = 0.1f)
        : Widget(bounds), text(initial_text), high_res_text(render_high_res_text), blink_timer(0.0f), show_cursor(true) {
        this->horizontal_padding = std::min(0.25f, std::max(0.0f, h_pad));
        this->vertical_padding = std::min(0.25f, std::max(0.0f, v_pad));
        this->is_focusable = true;
    }

    void set_text(const std::string& new_text){ text = new_text; }
    std::string get_text() const { return text; }

    void update(float delta_time = 0.0f) override {
        Widget::update(delta_time);

        if (is_focused){
            blink_timer += delta_time;
            if (blink_timer >= 0.5f){
                show_cursor = !show_cursor;
                blink_timer = 0.0f;
            }
        }
        else{
            show_cursor = false;
            blink_timer = 0.0f;
        }
    }

    void render(GUIContext& ctx) override {
        if (!is_visible) return;

        Rect old_clip = ctx.clip_rect;
        ctx.clip_rect = ctx.clip_rect.intersection(absolute_bounds);

        if ((ctx.clip_rect.x1 >= ctx.clip_rect.x2) || (ctx.clip_rect.y1 >= ctx.clip_rect.y2)){
            ctx.clip_rect = old_clip;
            return;
        }

        uint32_t body_color = (state == WidgetState::HOVERED) ? blend_color(ctx.theme.OTHER, 0xFFFFFFFF, 0.1) : ctx.theme.OTHER;
        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, body_color);

        uint32_t border_color = is_focused ? ctx.theme.PRIMARY_COLOR : ctx.theme.SECONDARY_COLOR;
        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x1 + 1, absolute_bounds.y2, border_color);
        draw_rectangle(ctx, absolute_bounds.x2 - 1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, border_color);
        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y1 + 1, border_color);
        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y2 - 1, absolute_bounds.x2, absolute_bounds.y2, border_color);

        float text_spacing_to_text_width_ratio = 0.125f;
        float text_height = static_cast<float>(absolute_bounds.y2 - absolute_bounds.y1 + 1) * (1.0f - 2.0f * vertical_padding);

        float char_size = text_height;
        float char_spacing = char_size * text_spacing_to_text_width_ratio;

        float cursor_x = static_cast<float>(absolute_bounds.x1) + static_cast<float>(absolute_bounds.x2 - absolute_bounds.x1 + 1) * horizontal_padding;
        uint32_t text_y1 = static_cast<uint32_t>(absolute_bounds.y1 + static_cast<float>(absolute_bounds.y2 - absolute_bounds.y1 + 1) * vertical_padding);
        uint32_t text_y2 = static_cast<uint32_t>(text_y1 + char_size);

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

        if (is_focused && show_cursor) {
            uint32_t cursor_width = std::max(2u, static_cast<uint32_t>(char_size * 0.1f));
            draw_rectangle(ctx, static_cast<uint32_t>(cursor_x), text_y1, static_cast<uint32_t>(cursor_x) + cursor_width, text_y2, ctx.theme.PRIMARY_COLOR);
        }

        ctx.clip_rect = old_clip;
    }

    Widget* handle_event(const GUIEvent& event) override {
        Widget* child_handled = Widget::handle_event(event);
        if (child_handled != nullptr) return child_handled;

        bool is_inside = absolute_bounds.contains(event.mouse_x, event.mouse_y);

        if (event.type == GUIEventType::MOUSE_MOVE){
            if (is_inside){
                state = WidgetState::HOVERED;
                return this;
            } else {
                state = WidgetState::NORMAL;
            }
        }
        else if (event.type == GUIEventType::MOUSE_BUTTON_DOWN && event.is_left_button){
            if (is_inside){
                state = WidgetState::PRESSED;
                // Force the cursor to blink visible immediately upon clicking
                show_cursor = true;
                blink_timer = 0.0f;
                return this;
            }
        }
        else if (event.type == GUIEventType::MOUSE_BUTTON_UP && event.is_left_button){
            if (is_inside){
                state = WidgetState::HOVERED;
                return this;
            }
        }
        else if (event.type == GUIEventType::KEY_DOWN && is_focused){
            if ((event.key_code == GUIKeyCode::BACKSPACE) && !text.empty()){
                text.pop_back();
            }
            return this;
        }
        else if ((event.type == GUIEventType::TEXT_INPUT) && is_focused){
            text += event.text_input;
            return this;
        }

        return nullptr;
    }
};

#endif