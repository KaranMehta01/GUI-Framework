#ifndef IMAGEBOX_H
#define IMAGEBOX_H

#include "Widget.h"
#include "RenderingFunctions.h"
#include "FundamentalTypes.h"

class ImageBox : public Widget {
private:
    ImageData* image;
    bool use_bilinear_filtering;

public:
    ImageBox(Rect bounds, ImageData* initial_image = nullptr, bool smooth_scaling = false)
        : Widget(bounds), image(initial_image), use_bilinear_filtering(smooth_scaling){
    }

    void set_image(ImageData* new_image){
        image = new_image;
    }

    void set_filtering(bool smooth){
        use_bilinear_filtering = smooth;
    }

    void render(GUIContext& ctx) override {
        if (!is_visible) return;

        Rect old_clip = ctx.clip_rect;
        ctx.clip_rect = ctx.clip_rect.intersection(absolute_bounds);

        if ((ctx.clip_rect.x1 >= ctx.clip_rect.x2) || (ctx.clip_rect.y1 >= ctx.clip_rect.y2)) {
            ctx.clip_rect = old_clip;
            return;
        }

        draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, ctx.theme.OTHER);

        if (image != nullptr && image->data.size() > 0) {
            if (use_bilinear_filtering) {
                draw_image_bilinear_interpolation(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, *image);
            } else {
                draw_image_nearest_neighbor(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, *image);
            }
        } else {
            draw_rectangle(ctx, absolute_bounds.x1, absolute_bounds.y1, absolute_bounds.x2, absolute_bounds.y2, 0xFFFF0000);
        }

        ctx.clip_rect = old_clip;
    }

    Widget* handle_event(const GUIEvent& event) override {
        Widget* child_handled = Widget::handle_event(event);
        if (child_handled != nullptr) return child_handled;

        // Does nothing for now

        return nullptr; 
    }
};

#endif