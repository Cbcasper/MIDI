//
// Created by Casper De Smet on 01/03/2022.
//

#include <iostream>
#include <string>

#include "Renderer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>


void Renderer::glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void Renderer::start() const
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    ImFontConfig config;
    config.OversampleH = 8;
    config.OversampleV = 8;
    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("./imgui/fonts/Roboto-Medium.ttf", 16.0f, &config);
    io.Fonts->AddFontFromFileTTF("./imgui/fonts/Roboto-Medium.ttf", 32.0f, &config);
    //io.Fonts->AddFontFromFileTTF("./imgui/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("./imgui/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("./imgui/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return;

    int count;
    std::string preferredMonitorName = "HP 27w";
    GLFWmonitor* preferredMonitor = glfwGetPrimaryMonitor();
    GLFWmonitor** monitor = glfwGetMonitors(&count);
    for (int i = 0; i < count; ++i)
    {
        if (preferredMonitorName == std::string(glfwGetMonitorName(*monitor)))
            preferredMonitor = *monitor;
        monitor++;
    }
    int xpos, ypos, width, height;
    glfwGetMonitorWorkarea(preferredMonitor, &xpos, &ypos, &width, &height);

    // Create window with graphics context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr)
        return;

    id <MTLDevice> device = MTLCreateSystemDefaultDevice();
    id <MTLCommandQueue> commandQueue = [device newCommandQueue];

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplMetal_Init(device);

    NSWindow *nsWindow = glfwGetCocoaWindow(window);
    CAMetalLayer *layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nsWindow.contentView.layer = layer;
    nsWindow.contentView.wantsLayer = YES;

    MTLRenderPassDescriptor *renderPassDescriptor = [MTLRenderPassDescriptor new];

    // Our state
    float clear_color[4] = {0.45f, 0.55f, 0.60f, 1.00f};

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        @autoreleasepool
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            layer.drawableSize = CGSizeMake(width, height);
            id<CAMetalDrawable> drawable = [layer nextDrawable];

            id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
            renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(clear_color[0] * clear_color[3], clear_color[1] * clear_color[3], clear_color[2] * clear_color[3], clear_color[3]);
            renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
            renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
            id <MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
            [renderEncoder pushDebugGroup:@"ImGui demo"];

            // Start the Dear ImGui frame
            ImGui_ImplMetal_NewFrame(renderPassDescriptor);
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            render();

            // Rendering
            ImGui::Render();
            ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

            [renderEncoder popDebugGroup];
            [renderEncoder endEncoding];

            [commandBuffer presentDrawable:drawable];
            [commandBuffer commit];
        }
    }

    // Cleanup
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}