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
    public:

        // This should be called every frame to update all entities containing a MovementComponent.
        void update(World* world, float deltaTime) {
            // For each entity in the
            PlayerComponent* player = nullptr;
            glm::vec3 playerPosition(0,0,0);
            for(auto entity : world->getEntities()){
                player = entity->getComponent<PlayerComponent>();
                if(player)
                {
                    playerPosition = entity->parent->localTransform.position+ entity->localTransform.position;
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
                    glm::vec3 target(0,0,0);
                    // different ghosts could have different targets for now all of them target the player
                    // if (movement->ghostId ==1)
                    target = playerPosition;

                    // move only in 1 of the 4 main directions after choosing the best one
                    glm::vec3 direction = glm::vec3(0, 0, -1); // UP
                    glm::vec3 bestDirection = direction; // inital value
                    Direction directionEnum;
                    float minDist = 3.402823E+38; // max float val
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

                    entity->localTransform.position += deltaTime * glm::length(movement->linearVelocity) * bestDirection;
                    movement->direction = directionEnum;

                    // notice that isCollided can be set to true in the play-state AFTER the movement system is called so the value used is the value from the previous frame
                    // and we need to reset isCollided to false for next frame
                    movement->isCollided = false;
                }
            }
        }

    };

}
