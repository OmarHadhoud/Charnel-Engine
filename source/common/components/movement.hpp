#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // This component denotes that the MovementSystem will move the owning entity by a certain linear and angular velocity.
    // This component is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/systems/movement.hpp"
    // For a more complex example of how to use the ECS framework, see "free-camera-controller.hpp"
    enum class Direction {
        DOWN = 0,
        RIGHT = 1,
        UP = 2,
        LEFT = 3,
        NONE,
    };

    class MovementComponent : public Component {
    public:
        glm::vec3 linearVelocity = {0, 0, 0}; // Each frame, the entity should move as follows: position += linearVelocity * deltaTime
        glm::vec3 angularVelocity = {0, 0, 0}; // Each frame, the entity should rotate as follows: rotation += angularVelocity * deltaTime

        // Ghost Ids are to represent which enemy this is. a default value of 0 means the movement doesn't change
        int ghostId = 0;
        Direction direction = Direction::RIGHT;
        glm::vec3 movementDirection = {1,0,0};
        const float baseMomentum = 1.5;
        float momentum = 0;
        Direction stuckDirection = Direction::NONE;
        bool isCollided = false;

        // chase/scatter modes
        // in chase mode ghosts target the player, and in scatter mode each one targets their corner
        bool isChase = true;
        const float baseChaseDuration = 40;
        float chaseDuration = baseChaseDuration;

        // The ID of this component type is "Movement"
        static std::string getID() { return "Movement"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}