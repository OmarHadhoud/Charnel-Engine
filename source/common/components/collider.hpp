#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // An enum that defines the type of the collider (BOX or Sphere) 
    enum class ColliderType {
        BOX,
        SPHERE
    };

    // This component represents the box collider that will be used to detect collision
    class ColliderComponent : public Component {
    public:
        ColliderType colliderType; // The type of the collider
        glm::vec3 minP; // The minimum point of the box collider
        glm::vec3 maxP; // The maximum point of the box collider
        glm::vec3 center; // The center for the sphere collider
        float radius; // The radius for the sphere collider

        // The ID of this component type is "Collider"
        static std::string getID() { return "Collider"; }

        // Reads box collider parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}