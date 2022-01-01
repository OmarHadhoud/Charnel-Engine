#pragma once


#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <components/player.hpp>
#include <asset-loader.hpp>

#include <irrKlang/include/irrklang.h>

// This state shows how to use the ECS framework and deserialization.
class Playstate: public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionSystem collisionSystem;
    irrklang::ISoundEngine* soundEngine;
    // the two entities representing the 2 digit score in the ui
    our::Entity *score1;
    our::Entity *score2;
    // the hearts representing the health of the player
    std::vector<our::Entity *> hearts;
    // the player entity
    our::Entity *player;
    // last time player got hit
    double lastHit;
    // rest time for player before being hit by ghosts again
    double restTime;
    // hack:
    // the ghost entities
    std::vector<our::Entity *> ghosts;
    double lastChange[4];
    double changeTime[4];

    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto& config = getApp()->getConfig()["game"];
        // If we have assets in the scene config, we deserialize them
        if(config.contains("assets")){
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if(config.contains("world")){
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        soundEngine = irrklang::createIrrKlangDevice();
        soundEngine->play2D("assets/sound/theme.mp3", true);
        soundEngine->setSoundVolume(0.1f);

        score1 = nullptr;
        score2 = nullptr;
        player = nullptr;
        hearts = std::vector<our::Entity *>(3, nullptr);
        // get score digit entities
        for (auto entity: world.getEntities())
        {
            if (entity->name == "score1")
            {
                score1 = entity;
            }
            else if(entity->name == "score2")
            {
                score2 = entity;
            } else if (entity->name == "player")
            {
                player = entity;
            } else if (entity->name == "heart1")
            {
                hearts[0] = entity;
            } else if (entity->name == "heart2")
            {
                hearts[1] = entity;
            } else if (entity->name == "heart3")
            {
                hearts[2] = entity;
            } else if(entity->name.size() > 5 && entity->name.substr(0,5) == "ghost")
                ghosts.push_back(entity);
            restTime = 3;
            changeTime[0] = rand() % 10  + 1;
            changeTime[1] = rand() % 10  + 1;
            changeTime[2] = rand() % 10  + 1;
            changeTime[3] = rand() % 10  + 1;
        }
    }

    void onDraw(double deltaTime) override {
        auto app = getApp();

        // Here, we just run a bunch of systems to control the world logic
        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);

        // get score and health for the player
        auto &score = player->getComponent<our::PlayerComponent>()->score;
        auto &health = player->getComponent<our::PlayerComponent>()->health;

        // drawing of score
        score1->getComponent<our::MeshRendererComponent>()->material = our::AssetLoader<our::Material>::get(std::to_string(score/10));
        score2->getComponent<our::MeshRendererComponent>()->material = our::AssetLoader<our::Material>::get(std::to_string(score%10));

        // drawing of hearts
        for (int i = 0; i < 3; i++)
        {
            hearts[i]->getComponent<our::MeshRendererComponent>()->enabled = i < health;
        }

        // And finally we use the renderer system to draw the scene
        auto size = getApp()->getFrameBufferSize();
        renderer.render(&world, glm::ivec2(0, 0), size);

        auto collisions = collisionSystem.update(&world, (float)deltaTime);
        for (auto collision: collisions)
        {
            auto entity1 = collision.first->getOwner();
            auto entity2 = collision.second->getOwner();
            if (entity1 == nullptr || entity2 == nullptr)
                continue;
            if (entity1 == player)
            {
                if (entity2->name.size() >= 4 && entity2->name.substr(0,4) == "maze")
                {
                    player->parent->localTransform = player->parent->prevLocalTransform;
                }
                else if (entity2->name.size() >= 4 && entity2->name.substr(0,4) == "coin")
                {
                    score++;
                    world.markForRemoval(entity2);
                    soundEngine->play2D("assets/sound/coin.mp3", false);
                }
                else if (entity2->name.size() >= 5 && entity2->name.substr(0,5) == "ghost")
                {
                    // TODO: maybe remove this
                    // player->parent->localTransform = player->parent->prevLocalTransform;
                    if (restTime > (glfwGetTime() - lastHit))
                        continue;
                    lastHit = glfwGetTime();
                    health--;
                    soundEngine->play2D("assets/sound/hit.wav", false);
                }
            }
            else if (entity1->name.size() >= 5 && entity1->name.substr(0,5) == "ghost")
            {
                if (entity2->name.size() >= 4 && entity2->name.substr(0,4) == "coin")
                    continue;
                if (entity2->name.size() >= 5 && entity2->name.substr(0,5) == "ghost")
                    continue; // disable collisions between ghosts
                entity1->localTransform = entity1->prevLocalTransform;

                if (auto ghostMovement = entity1->getComponent<our::MovementComponent>())
                {
                    // this value can be used in the movementSystem in the next frame
                    ghostMovement->isCollided = true;
                    // last collision direction is discarded from the movement options
                    ghostMovement->stuckDirection = ghostMovement->direction;
                }
            }
        }
        world.deleteMarkedEntities();
        // for (int i = 0; i < 4; i++)
        // {
        //     if (changeTime[i] > (glfwGetTime() - lastChange[i]))
        //         continue;
        //     ghosts[i]->getComponent<our::MovementComponent>()->linearVelocity = glm::vec3(rand() * 2, 0, rand() * 2);
        // }

        if (health <= 0)
            app->changeState("menu");
    }

    void onDestroy() override {
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
        soundEngine->drop();
    }
};