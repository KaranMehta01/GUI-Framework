#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H

#include <vector>
#include <memory>
#include <algorithm>

#include "FundamentalTypes.h"
#include "Widget.h"
#include "RenderingFunctions.h"

class RootWidget : public Widget {
public:
    RootWidget(Rect bounds) : Widget(bounds) {}

    void render(GUIContext& ctx) override {
        if (!is_visible) return;

        for (auto& child : children){
            child->render(ctx);
        }
    }
};

class GUIManager {
private:
    std::unique_ptr<Widget> root_canvas;
    Widget* focused_widget;
    Widget* captured_widget;

    std::vector<uint32_t> pixels;
    uint32_t width;
    uint32_t height;

    Theme current_theme;

public:
    GUIManager(uint32_t screen_width, uint32_t screen_height)
        : width(screen_width), height(screen_height), focused_widget(nullptr), captured_widget(nullptr)
    {
        pixels.resize(width * height, 0xFF000000);

        root_canvas = std::make_unique<RootWidget>(Rect(0, 0, width - 1, height - 1));
    }

    void set_theme(const Theme& theme){
        current_theme = theme;
    }

    void add_widget(std::unique_ptr<Widget> widget){
        root_canvas->add_child(std::move(widget));
    }

    void inject_event(const GUIEvent& event){
        if (((event.type == GUIEventType::KEY_DOWN) || (event.type == GUIEventType::TEXT_INPUT)) && focused_widget != nullptr){
            focused_widget->handle_event(event);
            return;
        }

        if ((captured_widget != nullptr) && (event.type == GUIEventType::MOUSE_MOVE || event.type == GUIEventType::MOUSE_BUTTON_UP)){
            captured_widget->handle_event(event);

            if (event.type == GUIEventType::MOUSE_BUTTON_UP){
                captured_widget = nullptr;
            }
            return;
        }

        Widget* handled_by = root_canvas->handle_event(event);

        if ((event.type == GUIEventType::MOUSE_BUTTON_DOWN)){
            if (focused_widget != handled_by){
                if (focused_widget != nullptr) focused_widget->is_focused = false;
                focused_widget = handled_by;
                if (focused_widget != nullptr) focused_widget->is_focused = true;
            }

            if (handled_by != nullptr){
                captured_widget = handled_by;
            }
        }
    }

    void update(float delta_time = 0.0f){
        root_canvas->update(delta_time);
    }

    void render(){
        std::fill(pixels.begin(), pixels.end(), current_theme.BACKGROUND_COLOR);

        GUIContext ctx{
            pixels,
            width,
            height,
            Rect(0, 0, width - 1, height - 1),
            current_theme
        };

        root_canvas->render(ctx);
    }

    const uint32_t* get_pixel_buffer() const {
        return pixels.data();
    }
};

#endif