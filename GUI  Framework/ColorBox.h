#ifndef COLOR_BOX_H
#define COLOR_BOX_H

#include "Widget.h"
#include "RenderingFunctions.h"

class ColorBox : public Widget {
private:
    uint32_t fill_color;

public:
    ColorBox(Rect bounds, uint32_t color = 0xFFFF0000)
        : Widget(bounds), fill_color(color) {
        this->is_focusable = false;
    }

    void set_color(uint32_t new_color) { fill_color = new_color; }
    uint32_t get_color() const { return fill_color; }

    void render(GUIContext& ctx) override {
        if (!is_visible) return;

        Rect old_clip = ctx.clip_rect;
        ctx.clip_rect = ctx.clip_rect.intersection(absolute_bounds);

        if ((ctx.clip_rect.x1 >= ctx.clip_rect.x2) || (ctx.clip_rect.y1 >= ctx.clip_rect.y2)) {
            ctx.clip_rect = old_clip;
            return;
        }

        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, fill_color);

        for (auto& child : children) {
            child->render(ctx);
        }

        ctx.clip_rect = old_clip;
    }

    Widget* handle_event(const GUIEvent& event) override {
        Widget* child_handled = Widget::handle_event(event);
        if (child_handled != nullptr) return child_handled;

        return nullptr; 
    }
};

#endif