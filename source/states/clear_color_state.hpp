#pragma once

#include <application.hpp>

class ClearColorState: public our::State {
    // onInitialize() function is called once before the state starts
    void onInitialize() override {
        // Read the color from the configuration file and use it to set the clear color for the window
        auto config = getApp()->getConfig();
        auto r = config["scene"]["clear-color"]["r"].get<float>();
        auto g = config["scene"]["clear-color"]["g"].get<float>();
        auto b = config["scene"]["clear-color"]["b"].get<float>();
        auto a = config["scene"]["clear-color"]["a"].get<float>();
        glClearColor(r, g, b, a);
    }

    // onDraw(deltaTime) function is called every frame 
    void onDraw(double deltaTime) override {
        //At the start of frame we want to clear the screen. Otherwise we would still see the results from the previous frame.
        glClear(GL_COLOR_BUFFER_BIT);
    }
};