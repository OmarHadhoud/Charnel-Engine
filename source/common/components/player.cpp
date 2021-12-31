#include "player.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    // Reads player component values
    void PlayerComponent::deserialize(const nlohmann::json& data){
        health = data.value("health", health);
        score = data.value("score", score);
    }
}