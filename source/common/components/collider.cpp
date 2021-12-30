#include "collider.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include "../deserialize-utils.hpp"

namespace our {
    // Reads box collider parameters from the given json object
    void ColliderComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string colliderTypeStr = data.value("colliderType", "sphere");
        if(colliderTypeStr == "box"){
            colliderType = ColliderType::BOX;
        } else {
            colliderType = ColliderType::SPHERE;
        }
        minP = data.value("min", minP);
        maxP = data.value("max", maxP);
        center = data.value("center", center);
        radius = data.value("radius", radius);
    }
}