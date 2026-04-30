#ifndef WIDGET_H
#define WIDGET_H

#include <vector>
#include <memory>

#include "FundamentalTypes.h"

class Widget{
protected:
    Widget* parent;
    std::vector<std::unique_ptr<Widget>> children;

    Rect local_bounds;
    Rect absolute_bounds;
    
    WidgetState state;
    bool is_visible;
    
public:
    bool is_focused;
    bool is_focusable;

    Widget(Rect bounds = Rect(0, 0, 0, 0)):
        parent(nullptr),
        local_bounds(bounds),
        absolute_bounds(bounds),
        state(WidgetState::NORMAL),
        is_visible(true),
        is_focused(false),
        is_focusable(false)
    {}

    virtual ~Widget() = default;

    virtual Rect get_client_bounds() const {
        return absolute_bounds;
    }

    virtual void set_visible(bool visible) { 
        is_visible = visible; 
    }
    
    bool get_visible() const { 
        return is_visible; 
    }

    virtual void update(float delta_time = 0.0f){
        if (!is_visible) return;

        if (parent != nullptr){
            Rect parent_client = parent->get_client_bounds();

            absolute_bounds.x1 = parent_client.x1 + local_bounds.x1;
            absolute_bounds.y1 = parent_client.y1 + local_bounds.y1;

            uint32_t width = local_bounds.x2 - local_bounds.x1;
            uint32_t height = local_bounds.y2 - local_bounds.y1;

            absolute_bounds.x2 = absolute_bounds.x1 + width;
            absolute_bounds.y2 = absolute_bounds.y1 + height;
        }
        else{
            absolute_bounds = local_bounds;
        }

        for (auto& child : children){
            child->update(delta_time);
        }
    }

    virtual void render(GUIContext& ctx) = 0;

    virtual Widget* handle_event(const GUIEvent& event){
        if (!is_visible) return nullptr;

        Widget* final_handled = nullptr;

        for (auto it = children.rbegin(); it != children.rend(); it++){            
            if (event.type == GUIEventType::MOUSE_BUTTON_DOWN){
                if ((*it)->absolute_bounds.contains(event.mouse_x, event.mouse_y)){
                    Widget* handled_by = (*it)->handle_event(event);
                    if (handled_by != nullptr) return handled_by;
                }
            }
            else if ((event.type == GUIEventType::MOUSE_BUTTON_UP) || (event.type == GUIEventType::MOUSE_MOVE))
            {
                Widget* handled_by = (*it)->handle_event(event);
                if ((handled_by != nullptr) && (final_handled == nullptr)){
                    final_handled = handled_by;
                }
            }
            else{
                Widget* handled_by = (*it)->handle_event(event);
                if (handled_by != nullptr) return handled_by;
            }
        }

        return final_handled;
    }

    void add_child(std::unique_ptr<Widget> child){
        if (child) {
            child->parent = this;
            children.push_back(std::move(child));
        }
    }

    void set_local_bounds(const Rect& bounds){local_bounds = bounds;}
    Rect get_absolute_bounds() const {return absolute_bounds;}
};

#endif