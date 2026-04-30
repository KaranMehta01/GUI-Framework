#ifndef FUNDAMENTAL_TYPES_H
#define FUNDAMENTAL_TYPES_H

#include <cstdint>
#include <vector>

enum class WidgetState : unsigned char{
    NORMAL, HOVERED, PRESSED
};

enum class GUIEventType : unsigned char{
    MOUSE_BUTTON_DOWN,
    MOUSE_BUTTON_UP,
    MOUSE_MOVE,
    KEY_DOWN,
    KEY_UP,
    TEXT_INPUT
};

enum class GUIKeyCode : unsigned int{
    UNKNOWN,
    BACKSPACE,
    ENTER
};

struct GUIEvent{
    GUIEventType type;
    uint32_t mouse_x, mouse_y;
    bool is_left_button;
    GUIKeyCode key_code;
    char text_input;
};

struct Rect{
    uint32_t x1, y1, x2, y2;

    Rect(uint32_t x1 = 0, uint32_t y1 = 0, uint32_t x2 = 0, uint32_t y2 = 0){
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }

    bool contains(uint32_t px, uint32_t py) const {
        return ((px >= x1) && (px <= x2)) && ((py >= y1) && (py <= y2));
    }

    Rect intersection(const Rect& other){
        Rect intersecting_rectangle(0,0,0,0);

        if ((x2 >= other.x1) && (y2 >= other.y1) && (x1 <= other.x2) && (y1 <= other.y2)){
            intersecting_rectangle.x1 = (x1 > other.x1) ? x1 : other.x1;
            intersecting_rectangle.y1 = (y1 > other.y1) ? y1 : other.y1;
            intersecting_rectangle.x2 = (x2 < other.x2) ? x2 : other.x2;
            intersecting_rectangle.y2 = (y2 < other.y2) ? y2 : other.y2;
        }

        return intersecting_rectangle;
    }
};

struct Theme{
    uint32_t BACKGROUND_COLOR;
    uint32_t FOREGROUND_COLOR;
    uint32_t PRIMARY_COLOR;
    uint32_t SECONDARY_COLOR;
    uint32_t OTHER;

    Theme(uint32_t BACKGROUND_COLOR = 0xFF228888, uint32_t FOREGROUND_COLOR = 0xFF220000,
          uint32_t PRIMARY_COLOR = 0xFF99FFEE, uint32_t SECONDARY_COLOR = 0xFF225555,
          uint32_t OTHER = 0xFF44BBBB){
        this->BACKGROUND_COLOR = BACKGROUND_COLOR;
        this->FOREGROUND_COLOR = FOREGROUND_COLOR;
        this->PRIMARY_COLOR = PRIMARY_COLOR;
        this->SECONDARY_COLOR = SECONDARY_COLOR;
        this->OTHER = OTHER;
    }
};

struct ImageData{
    std::vector<uint32_t>& data;
    uint32_t width, height;

    ImageData(std::vector<uint32_t>& data, uint32_t width, uint32_t height)
        : data(data), width(width), height(height) {}
};

struct GUIContext{
    std::vector<uint32_t>& pixels;
    uint32_t screen_width, screen_height;
    Rect clip_rect;
    const Theme& theme;
};

#endif