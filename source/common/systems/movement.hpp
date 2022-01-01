#pragma once

#include "../ecs/world.hpp"
#include "../components/movement.hpp"
#include "../components/player.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/components/movement.hpp"
    class MovementSystem {
        glm::vec3 blinkyPosition = {0,0,0};
    public:

        // This should be called every frame to update all entities containing a MovementComponent.
        void update(World* world, float deltaTime) {
            // For each entity in the
            PlayerComponent* player = nullptr;
            glm::vec3 playerPosition(0,0,0);
            glm::vec3 cameraForward(0,0,0);
            for(auto entity : world->getEntities()){
                player = entity->getComponent<PlayerComponent>();
                if(player)
                {
                    playerPosition = entity->parent->getLocalToWorldMatrix() * glm::vec4(entity->localTransform.position, 1.0f);
                    cameraForward =  entity->parent->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0);
                    cameraForward.y=0;
                    cameraForward = glm::normalize(cameraForward);
                    break;
                }
            }

            for(auto entity : world->getEntities()){
                // store previous positions even if no movement component
                entity->prevLocalTransform = entity->localTransform;
                // Get the movement component if it exists
                MovementComponent* movement = entity->getComponent<MovementComponent>();
                // If the movement component exists
                if(movement && movement->enabled){
                    // Change the position and rotation based on the linear & angular velocity and delta time.
                    if (movement->ghostId == 0)
                    {
                        entity->localTransform.position += deltaTime * movement->linearVelocity;
                        entity->localTransform.rotation += deltaTime * movement->angularVelocity;
                        continue;
                    }

                    // enemy movement logic depends on the player location
                    if (player == nullptr) continue;

                    if (movement->chaseDuration <=0)
                    {
                        // toggle between modes
                        movement->isChase = !movement->isChase;
                        movement->chaseDuration = movement->baseChaseDuration;
                        // scatter for less time
                        if (movement->isChase == false)
                            movement->chaseDuration *= 1/2.0f;
                    }

                    glm::vec3 target(0,0,0);
                    float distance = glm::length (playerPosition - entity->localTransform.position);
                    // different ghosts have different targets
                    if (movement->ghostId ==1)
                    {
                        if (movement->isChase)
                        {
                            // Blinky targets pacman
                            target = playerPosition;
                        }
                        else
                            target = glm::vec3(5, 0, -5);

                        blinkyPosition = entity->localTransform.position;
                    }
                    else if (movement->ghostId ==2)
                    {
                        if (movement->isChase)
                        {
                            // Pinky targets 3 steps ahead of pacman
                            // optional: targets pacman when close
                            if (distance > 3.0f) distance = 3; // if further than 3 target 3 ahead
                            target = playerPosition + distance * cameraForward;
                        }
                        else
                            target = glm::vec3(-5, 0, -5);

                    }
                    else if (movement->ghostId ==3)
                    {
                        if (movement->isChase)
                        {
                            // Inky flanks pacman with blinky
                            // choose a point infront of pacman
                            target = playerPosition + 1.0f *  cameraForward;
                            // target the opposite side of blinky
                            target = 2.0f * target - blinkyPosition;
                        }
                        else
                            target = glm::vec3(5, 0, 5);
                    }
                    else if (movement->ghostId ==4)
                    {
                        if (movement->isChase)
                        {
                            // Clyde targets pacman until he's close then targets his own corner in the map
                            target = distance > 3.0f ? playerPosition: glm::vec3(-5, 0, 5);
                        }
                        else
                            target = glm::vec3(-5, 0, 5);

                    }

                    // move only in 1 of the 4 main directions after choosing the best one
                    glm::vec3 bestDirection = movement->movementDirection; // inital value
                    Direction directionEnum = movement->direction ;
                    float minDist = (float)3.402823E+38; // max float val
                    // only change direction if momentum reached 0 or if collided
                    if (movement->isCollided || movement->momentum <= 0)
                    {
                        // reset locked direction if not collided and momentum reached 0
                        if(movement->isCollided == false)
                            movement->stuckDirection= Direction::NONE;

                        movement->momentum = movement->baseMomentum; // reset momentum value
                        glm::vec3 direction = glm::vec3(0, 0, -1); // UP
                        if ((minDist > glm::length(target - (entity->localTransform.position + direction))) && movement->stuckDirection != Direction::UP)
                        {
                            minDist = glm::length(target - (entity->localTransform.position + direction));
                            bestDirection = direction;
                            directionEnum = Direction::UP;
                        }
                        direction = glm::vec3(1, 0, 0); // Right
                        if ((minDist > glm::length(target - (entity->localTransform.position + direction))) && movement->stuckDirection != Direction::RIGHT)
                        {
                            minDist = glm::length(target - (entity->localTransform.position + direction));
                            bestDirection = direction;
                            directionEnum = Direction::RIGHT;
                        }
                        direction = glm::vec3(0, 0, 1);  // Down
                        if ((minDist > glm::length(target - (entity->localTransform.position + direction))) && movement->stuckDirection != Direction::DOWN)
                        {
                            minDist = glm::length(target - (entity->localTransform.position + direction));
                            bestDirection = direction;
                            directionEnum = Direction::DOWN;
                        }
                        direction = glm::vec3(-1, 0, 0); // Left
                        if ((minDist > glm::length(target - (entity->localTransform.position + direction))) && movement->stuckDirection != Direction::LEFT)
                        {
                            minDist = glm::length(target - (entity->localTransform.position + direction));
                            bestDirection = direction;
                            directionEnum = Direction::LEFT;
                        }
                    }

                    entity->localTransform.position += deltaTime * glm::length(movement->linearVelocity) * bestDirection;
                    entity->localTransform.rotation.y = glm::pi<float>() * int(directionEnum) / 2;
                    movement->momentum -= deltaTime * glm::length(movement->linearVelocity);
                    movement->chaseDuration -= deltaTime * glm::length(movement->linearVelocity);
                    movement->direction = directionEnum;
                    movement->movementDirection = bestDirection;

                    // notice that isCollided can be set to true in the play-state AFTER the movement system is called so the value used is the value from the previous frame
                    // and we need to reset isCollided to false for next frame
                    movement->isCollided = false;
                }
            }
        }

    };

}
