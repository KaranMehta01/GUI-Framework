# GUI Framework using C++ 
A lightweight, backend-agnostic, software-rendered GUI framework written in modern C++. 
This framework is designed to be completely independent of any specific windowing or graphics 
library (like SDL, SFML, GLFW, or DirectX/OpenGL). It renders all UI elements directly into a 1D 
pixel buffer, making it incredibly easy to drop into any existing project. If your environment can 
open a window, poll input events, and draw an array of pixels to the screen, you can use this 
framework. 

# Features 
● **Zero Dependencies**: Uses only the C++ standard library (\<vector>, \<memory>, \<string>, 
\<functional>, etc.). 

● **Backend Agnostic**: Bring your own window and input handler. 

● **Built-in Software Renderer**: Handles drawing rectangles, color blending, and image 
scaling (Nearest Neighbor & Bilinear Interpolation) without hardware acceleration. 

● **UI Clipping**: Rendering is strictly clipped to widget boundaries, perfect for container panels. 

● **Event Routing**: Smart event bubbling, focus management, and widget capturing (e.g., for 
dragging sliders or panels). 

● **Theming Engine**: Easily customize background, foreground, primary, and secondary 
colors globally. 

● **Built-in Fonts**: Comes with zero-dependency 7x7 and 16x16 pixel glyphs for immediate 
text rendering. 

# Available Widgets 
● **Widget (Base)**: The core class handling visibility, bounds, state, and child management. 

● **Button**: Clickable button with hover/press states and text rendering. 

● **Label**: Simple static text display. 

● **TextBox**: Focusable input field with blinking cursor and backspace support. 

● **Panel**: A container widget with a title bar that can act as a draggable window. 

● **Slider**: A draggable value slider with customizable minimum, maximum, and knob ratios. 

● **ImageBox**: Displays pixel-buffer images with optional bilinear filtering. 

● **ColorBox**: A simple widget that renders a solid color block. 

# Quick Start / Integration 
Integrating the framework requires bridging your chosen backend (e.g., SDL2, SFML) to the 
GUIManager. 
1. **Initialization**

Create the GUI Manager and pass it the dimensions of your screen/window. 
```
#include "GUIManager.h" 
#include "Button.h" 
#include "Panel.h"

// Initialize the GUI Manager with window dimensions 
uint32_t screen_width = 800; 
uint32_t screen_height = 600; 
GUIManager gui(screen_width, screen_height);

// Setup a Theme 
Theme my_theme(0xFF222222, 0xFFFFFFFF, 0xFF007ACC, 0xFF333333, 0xFF555555); 
gui.set_theme(my_theme);

// Create a draggable Panel 
auto my_panel = std::make_unique<Panel>(Rect(100, 100, 400, 300), "My Window");

// Add a Button to the panel (local coordinates relative to panel's client area) 
auto my_button = std::make_unique<Button>(Rect(10, 10, 150, 50), "Click Me!"); 
my_button->set_on_click([]() { 
    // Button clicked logic! 
});

my_panel->add_child(std::move(my_button)); 
gui.add_widget(std::move(my_panel));
```
2. **The Main Loop**

In your application's main loop, you need to translate your backend's input events into GUIEvent 
objects, update the GUI, render it, and finally draw the pixel buffer to your screen. 
```
bool running = true; 
float delta_time = 0.016f; // Calculate this based on your frame times 
while (running) { 
    // --- 1. HANDLE INPUT --- 
    // Example: Translate your backend's events (SDL_Event, sf::Event, etc.) 
    GUIEvent my_event;
 
    // ... logic to populate 'my_event' ... 
    // my_event.type = GUIEventType::MOUSE_MOVE; 
    // my_event.mouse_x = ...; 
    // my_event.mouse_y = ...; 
     
    // Inject the event into the framework 
    gui.inject_event(my_event); 
 
    // --- 2. UPDATE --- 
    // Updates internal timers (like the TextBox cursor blink) and bounding boxes 
    gui.update(delta_time); 
 
    // --- 3. RENDER --- 
    // Clears the buffer and draws all widgets into the internal pixel array 
    gui.render(); 
 
    // --- 4. PRESENT --- 
    // Retrieve the raw pixel data 
    const uint32_t* raw_pixels = gui.get_pixel_buffer(); 
     
    // --> Copy `raw_pixels` to your backend's texture/screen surface here! <-- 
}
```
 
# Architecture 
1. **FundamentalTypes.h**: Defines the vocabulary of the framework (Rect, GUIEvent, Theme, 
ImageData). It uses standard 32-bit integers (uint32_t) for colors formatted typically as 
ARGB/ABGR depending on your backend interpretation. 
2. **RenderingFunctions.h**: The heart of the software renderer. All drawing operations 
guarantee memory safety by strictly adhering to the clip_rect supplied by the GUIContext. 
3. **Event Propagation**: Events are injected into the GUIManager, which passes them to the 
RootWidget. Events are routed back-to-front (children first) to ensure elements drawn on 
top receive clicks first. The manager automatically handles focused_widget (for keyboard 
input) and captured_widget (for drag operations that leave the widget's physical bounds). 
