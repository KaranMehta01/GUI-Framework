#ifndef LABEL_H
#define LABEL_H

#include <string>
#include "Widget.h"
#include "RenderingFunctions.h"
#include "Glyphs7X7.h"
#include "Glyphs16X16.h"

class Label : public Widget {
private:
    std::string text;
    bool high_res_text;

public:
    Label(Rect bounds, const std::string& label_text = "", bool render_high_res = false, uint32_t color = 0xFFFFFFFF)
        : Widget(bounds), text(label_text), high_res_text(render_high_res){
        this->is_focusable = false;
    }

    void set_text(std::string new_text) { text = new_text; }
    std::string get_text() const { return text; }

    void render(GUIContext& ctx) override {
        if (!is_visible || text.empty()) return;

        Rect old_clip = ctx.clip_rect;
        ctx.clip_rect = ctx.clip_rect.intersection(absolute_bounds);

        if ((ctx.clip_rect.x1 >= ctx.clip_rect.x2) || (ctx.clip_rect.y1 >= ctx.clip_rect.y2)) {
            ctx.clip_rect = old_clip;
            return;
        }

        float text_spacing_to_text_width_ratio = 0.125f;

        float no_of_characters = text.size();

        float char_size = static_cast<float>(absolute_bounds.x2 - absolute_bounds.x1 + 1) / (no_of_characters + text_spacing_to_text_width_ratio * (no_of_characters - 1.0));
        float char_spacing = char_size * text_spacing_to_text_width_ratio;

        float cursor_x = static_cast<float>(absolute_bounds.x1);

        if (high_res_text){
            for (char c : text) {
                const bool (&glyph)[16][16] = get_glyph_16x16(c);
                
                draw_high_res_glyph(ctx, cursor_x, absolute_bounds.y1, cursor_x + char_size - 1, absolute_bounds.y2, glyph, ctx.theme.FOREGROUND_COLOR);
                
                cursor_x += (char_size + char_spacing);
            }
        }
        else{
            for (char c : text) {
                const bool (&glyph)[7][7] = get_glyph_7x7(c);
                
                draw_glyph(ctx, cursor_x, absolute_bounds.y1, cursor_x + char_size - 1, absolute_bounds.y2, glyph, ctx.theme.FOREGROUND_COLOR);
                
                cursor_x += (char_size + char_spacing);
            }
        }

        ctx.clip_rect = old_clip;
    }

    Widget* handle_event(const GUIEvent& event) override {
        return nullptr;
    }
};

#endif