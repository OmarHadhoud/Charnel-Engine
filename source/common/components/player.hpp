#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // This is the player component. It contains the health and the score of the player.
    class PlayerComponent : public Component {
    public:
        int health = 3;
        int score = 0;

        // The ID of this component type is "Player"
        static std::string getID() { return "Player"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}