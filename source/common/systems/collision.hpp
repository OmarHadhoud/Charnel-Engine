#pragma once

#include "../ecs/world.hpp"
#include "../components/collider.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

#include <vector>
#include <utility>

namespace our
{

    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic. 
    // For more information, see "common/components/movement.hpp"
    class CollisionSystem {
    private:
        bool BoxCollideBox(ColliderComponent *collider1, ColliderComponent *collider2){
            // Check collision between the 2 entities using AABB
            auto entity = collider1->getOwner();
            auto entity2 = collider2->getOwner();
            auto min1 = entity->getLocalToWorldMatrix() * glm::vec4(collider1->minP, 1.0f);
            auto max1 = entity->getLocalToWorldMatrix() * glm::vec4(collider1->maxP, 1.0f);
            auto min2 = entity2->getLocalToWorldMatrix() * glm::vec4(collider2->minP, 1.0f);
            auto max2 = entity2->getLocalToWorldMatrix() * glm::vec4(collider2->maxP, 1.0f);
            return (min1.x <= max2.x &&
                    max1.x >= min2.x &&
                    min1.y <= max2.y &&
                    max1.y >= min2.y &&
                    min1.z <= max2.z &&
                    max1.z >= min2.z);
        };

        bool BoxCollideSphere(ColliderComponent *collider1, ColliderComponent *collider2){
            auto entity = collider1->getOwner();
            auto entity2 = collider2->getOwner();
            auto min1 = entity->getLocalToWorldMatrix() * glm::vec4(collider1->minP, 1.0f);
            auto max1 = entity->getLocalToWorldMatrix() * glm::vec4(collider1->maxP, 1.0f);
            auto center = entity2->getLocalToWorldMatrix() * glm::vec4(collider2->center, 1.0f);
            double dmin = 0;
            for(int i = 0; i < 3; i++ ) {
                if( center[i] < min1[i] )
                    dmin += (center[i] - min1[i]) * (center[i] - min1[i]);
                else if( center[i] > max1[i] )
                     dmin += (center[i] - max1[i]) * (center[i] - max1[i]);
                }
            return(dmin <= (collider2->radius * collider2->radius));
        };

        bool SphereCollideSphere(ColliderComponent *collider1, ColliderComponent *collider2){
            return false;
            auto entity = collider1->getOwner();
            auto entity2 = collider2->getOwner();
            auto center1 = entity->getLocalToWorldMatrix() * glm::vec4(collider1->center, 1.0f);
            auto center2 = entity2->getLocalToWorldMatrix() * glm::vec4(collider2->center, 1.0f);
            return glm::dot(center1 - center2, center1 - center2) <= (collider1->radius * collider1->radius) + (collider2->radius *collider2->radius);
        };

    public:

        // This should be called every frame to update all entities containing a MovementComponent. 
        std::vector<std::pair<ColliderComponent*, ColliderComponent*>>
        update(World* world, float deltaTime) {
            std::vector<std::pair<ColliderComponent*, ColliderComponent*>> collisions;
            // For each entity in the world
            for(auto entity : world->getEntities())
            {
                // Get the box collider component if it exists
                ColliderComponent* collider1 = entity->getComponent<ColliderComponent>();
                // If the box collider component doesn't exist, continue
                if (!collider1)
                    continue;
                for(auto entity2 : world->getEntities())
                {
                    // Get the box collider component if it exists
                    ColliderComponent* collider2 = entity2->getComponent<ColliderComponent>();
                    if (entity == entity2 || collider2 == nullptr)
                        continue;
                    if (collider1->colliderType == ColliderType::BOX && collider2->colliderType == ColliderType::BOX)
                    {
                        if (BoxCollideBox(collider1, collider2))
                            collisions.push_back(std::make_pair(collider1, collider2));
                    }
                    else if (collider1->colliderType == ColliderType::SPHERE && collider2->colliderType == ColliderType::SPHERE)
                    {
                        if (SphereCollideSphere(collider1, collider2))
                            collisions.push_back(std::make_pair(collider1, collider2));
                    }
                    else if(collider1->colliderType == ColliderType::BOX)
                    {
                        if (BoxCollideSphere(collider1, collider2))
                            collisions.push_back(std::make_pair(collider1, collider2));
                    }
                    else
                    {
                        if (BoxCollideSphere(collider2, collider1))
                            collisions.push_back(std::make_pair(collider1, collider2));
                    }
                }
            }
            return collisions;
        }

    };

}
