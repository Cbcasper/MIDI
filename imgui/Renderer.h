//
// Created by Casper De Smet on 27/02/2022.
//

#ifndef MIDIASSISTANT_RENDERER_H
#define MIDIASSISTANT_RENDERER_H

#include <functional>
#include <utility>
#include <string>

#include "imgui.h"

#import "../UI/Font.h"

class Renderer
{
public:
    std::function<void()> render;
    const char* title;

    explicit Renderer(std::function<void()> render, const char* title): render(std::move(render)), title(title) {}

    static void glfw_error_callback(int error, const char* description);
    void start(UI::FontPointer& defaultFont, UI::FontPointer& mediumFont, UI::FontPointer& largeFont) const;
};


#endif //MIDIASSISTANT_RENDERER_H
