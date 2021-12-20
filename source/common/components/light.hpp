#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // An enum that defines the type of the Light (Direct, Point, Spot) 
    enum class LightType {
        DIRECT,
        POINT,
        SPOT
    };

    // This is the light component. It contains the data that defines the light.
    class LightComponent : public Component {
    public:
        LightType lightType; // The type of the light

        // data needed for all light types
        glm::vec3 diffuse; // The diffuse color of the light
        glm::vec3 specular; // The specular color of the light
        glm::vec3 ambient; // The ambient color of the light

        // data needed for point light type
        float attenuationConstant; // The constant attenuation factor
        float attenuationLinear; // The linear attenuation factor
        float attenuationQuadratic; // The quadratic attenuation factor

        // data needed for spot light
        float innerCutoff; // The inner cutoff angle of the spot light
        float outerCutoff; // The outer cutoff angle of the spot light

        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}