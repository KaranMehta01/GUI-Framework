#ifndef PANEL_H
#define PANEL_H

#include "Widget.h"
#include "RenderingFunctions.h"
#include <string>
#include <algorithm>

class Panel : public Widget{
private:
    std::string title;
    float title_height_ratio;  // title height to panel height ratio

    bool is_draggable;
    bool is_dragging;
    
    int32_t drag_offset_x;
    int32_t drag_offset_y;

    bool high_res_text;
    float horizontal_padding;
    float vertical_padding;

public:
    Panel(Rect bounds, const std::string& panel_title = "Window", bool draggable = true, float title_height_ratio = 0.1, bool render_high_res_text = true, float h_pad = 0.025f, float v_pad = 0.1f)
        : Widget(bounds), title(panel_title), is_draggable(draggable), is_dragging(false), drag_offset_x(0), drag_offset_y(0), high_res_text(render_high_res_text){
        this->title_height_ratio = std::min(0.5f, std::max(0.05f, title_height_ratio));
        if (!is_draggable){
            this->title_height_ratio = std::min(0.5f, std::max(0.00f, title_height_ratio));
        }
        this->horizontal_padding = std::min(0.25f, std::max(0.0f, h_pad));
        this->vertical_padding = std::min(0.25f, std::max(0.0f, v_pad));
    }

    Rect get_client_bounds() const override {
        uint32_t title_height = static_cast<uint32_t>(static_cast<float>(absolute_bounds.y2 - absolute_bounds.y1 + 1) * title_height_ratio);
        return Rect(absolute_bounds.x1 + 2, absolute_bounds.y1 + title_height + 1, absolute_bounds.x2 - 2, absolute_bounds.y2 - 1);
    }

    void render(GUIContext& ctx) override {
        if (!is_visible) return;

        Rect old_clip = ctx.clip_rect;
        ctx.clip_rect = ctx.clip_rect.intersection(absolute_bounds);

        if ((ctx.clip_rect.x1 >= ctx.clip_rect.x2) || (ctx.clip_rect.y1 >= ctx.clip_rect.y2)){
            ctx.clip_rect = old_clip;
            return;
        }

        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, ctx.theme.OTHER);

        uint32_t title_height = static_cast<uint32_t>(static_cast<float>(absolute_bounds.y2 - absolute_bounds.y1 + 1) * title_height_ratio);
        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y1 + title_height - 1, ctx.theme.SECONDARY_COLOR);

        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x1 + 1, absolute_bounds.y2, ctx.theme.FOREGROUND_COLOR);
        draw_rectangle(ctx, absolute_bounds.x2 - 1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, ctx.theme.FOREGROUND_COLOR);
        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y1 + 1, ctx.theme.FOREGROUND_COLOR);
        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y2 - 1, absolute_bounds.x2, absolute_bounds.y2, ctx.theme.FOREGROUND_COLOR);

        float text_spacing_to_text_width_ratio = 0.125f;
        float text_height = title_height * (1.0f - 2.0f * vertical_padding);

        float char_size = text_height;
        float char_spacing = char_size * text_spacing_to_text_width_ratio;

        float cursor_x = static_cast<float>(absolute_bounds.x1) + static_cast<float>(absolute_bounds.x2 - absolute_bounds.x1 + 1) * horizontal_padding;
        uint32_t text_y1 = static_cast<uint32_t>(absolute_bounds.y1 + title_height * vertical_padding);
        uint32_t text_y2 = static_cast<uint32_t>(text_y1 + char_size);

        if (high_res_text){
            for (char c : title) {
                const bool (&glyph)[16][16] = get_glyph_16x16(c);
                draw_high_res_glyph(ctx, cursor_x, text_y1, cursor_x + char_size - 1, text_y2, glyph, ctx.theme.FOREGROUND_COLOR);
                cursor_x += (char_size + char_spacing);
            }
        }
        else{
            for (char c : title) {
                const bool (&glyph)[7][7] = get_glyph_7x7(c);
                draw_glyph(ctx, cursor_x, text_y1, cursor_x + char_size - 1, text_y2, glyph, ctx.theme.FOREGROUND_COLOR);
                cursor_x += (char_size + char_spacing);
            }
        }

        Rect body_rect = get_client_bounds();
        ctx.clip_rect = ctx.clip_rect.intersection(body_rect);

        for (auto& child : children){
            child->render(ctx);
        }

        ctx.clip_rect = old_clip;
    }

    Widget* handle_event(const GUIEvent& event) override {
        uint32_t title_height = static_cast<uint32_t>(static_cast<float>(absolute_bounds.y2 - absolute_bounds.y1 + 1) * title_height_ratio);
        bool is_inside = absolute_bounds.contains(event.mouse_x, event.mouse_y);
        bool is_in_title_bar = is_inside && (event.mouse_y <= absolute_bounds.y1 + title_height - 1);

        if (is_dragging && (event.type == GUIEventType::MOUSE_MOVE || event.type == GUIEventType::MOUSE_BUTTON_UP)) {
            // Do nothing here, let it fall through to the Panel logic below
        }
        else if (event.type == GUIEventType::MOUSE_BUTTON_DOWN && event.is_left_button && is_in_title_bar && is_draggable) {
            // Do nothing here, let it fall through to the Panel logic below
        }
        else {
            Widget* child_handled = Widget::handle_event(event);
            if (child_handled != nullptr) return child_handled;
        }

        if (event.type == GUIEventType::MOUSE_MOVE){
            if (is_dragging){
                int32_t new_abs_x = static_cast<int32_t>(event.mouse_x) - drag_offset_x;
                int32_t new_abs_y = static_cast<int32_t>(event.mouse_y) - drag_offset_y;

                int32_t parent_client_x = parent ? static_cast<int32_t>(parent->get_client_bounds().x1) : 0;
                int32_t parent_client_y = parent ? static_cast<int32_t>(parent->get_client_bounds().y1) : 0;

                int32_t parent_client_width = parent ? static_cast<int32_t>(parent->get_client_bounds().x2 - parent->get_client_bounds().x1 + 1) : 99999;
                int32_t parent_client_height = parent ? static_cast<int32_t>(parent->get_client_bounds().y2 - parent->get_client_bounds().y1 + 1) : 99999;

                int32_t width = static_cast<int32_t>(local_bounds.x2 - local_bounds.x1);
                int32_t height = static_cast<int32_t>(local_bounds.y2 - local_bounds.y1);

                int32_t target_local_x = new_abs_x - parent_client_x;
                int32_t target_local_y = new_abs_y - parent_client_y;

                int32_t max_local_x = std::max(0, parent_client_width - width);
                int32_t max_local_y = std::max(0, parent_client_height - height);

                local_bounds.x1 = static_cast<uint32_t>(std::max(0, std::min(target_local_x, max_local_x)));
                local_bounds.y1 = static_cast<uint32_t>(std::max(0, std::min(target_local_y, max_local_y)));

                local_bounds.x2 = local_bounds.x1 + static_cast<uint32_t>(width);
                local_bounds.y2 = local_bounds.y1 + static_cast<uint32_t>(height);

                return this;
            }

            if (is_inside){
                return this;
            }
        }
        else if ((event.type == GUIEventType::MOUSE_BUTTON_DOWN) && event.is_left_button){
            if (is_in_title_bar && is_draggable){
                is_dragging = true;

                drag_offset_x = static_cast<int32_t>(event.mouse_x) - static_cast<int32_t>(absolute_bounds.x1);
                drag_offset_y = static_cast<int32_t>(event.mouse_y) - static_cast<int32_t>(absolute_bounds.y1);
                return this;
            }
            else if (is_inside){
                return this;
            }
        }
        else if ((event.type == GUIEventType::MOUSE_BUTTON_UP) && event.is_left_button){
            if (is_dragging){
                is_dragging = false;
                return this;
            }
            if (is_inside){
                return this;
            }
        }

        return nullptr;
    }
};

#endif