#pragma once


#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <asset-loader.hpp>

#include <irrKlang/include/irrklang.h>

// This state shows how to use the ECS framework and deserialization.
class Menustate: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    irrklang::ISoundEngine* soundEngine;
    our::Entity* pacman_select;
    bool select_play = true;

    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["menu"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }

        // initialize the sound engine
        soundEngine = irrklang::createIrrKlangDevice();
        soundEngine->play2D("assets/sound/theme.mp3", true);
        soundEngine->setSoundVolume(0.1f);

        // get the pacman select entity
        for (auto entity: world.getEntities())
        {
            if (entity->name == "pacman_select")
            {
                pacman_select = entity;
                break;
            }
        }
    }

    void onDraw(double deltaTime) override {
        auto app = getApp();
        if(app->getKeyboard().isPressed(GLFW_KEY_UP) && !select_play)
        {
            select_play = true;
            pacman_select->localTransform.position.y = 1.0f;
        }
        if(app->getKeyboard().isPressed(GLFW_KEY_DOWN) && select_play)
        {
            select_play = false;
            pacman_select->localTransform.position.y = -3.0f;
        }
        if(app->getKeyboard().isPressed(GLFW_KEY_SPACE))
        {
            if (select_play)
                app->changeState("play");
            else
                glfwSetWindowShouldClose(app->getWindow(), GLFW_TRUE);
        }
        // we use the renderer system to draw the scene
        auto size = getApp()->getFrameBufferSize();
        renderer.render(&world, glm::ivec2(0, 0), size);
    }

    void onDestroy() override {
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
        soundEngine->drop();
    }
};