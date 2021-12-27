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

        // data needed for point/spot light types
        // when light is coming from a close source, the intensity goes down the further
        // we go from this source, this is called attenuation. if we use only a linear equation it will
        // feel fake, so we use a quadratic equation (similar to inverse law in real life)
        // Fatt = 1 / (kc + kl * d + kq * d^2), we use kc to be 1 usually to make sure denominator is never less than 1
        // in close distance, kl * d will dominate so it goes down linearlly, until d is large enough for quadratic term to be dominant
        float attenuationConstant; // The constant attenuation factor
        float attenuationLinear; // The linear attenuation factor
        float attenuationQuadratic; // The quadratic attenuation factor

        // data needed for spot light
        // when light is coming from a spot light, it usually has a cone-like effect as it goes from the source to the target
        // we can use only one angle for this, but it will look fake, as it will have hard edges, so we use two angles to make it 
        // look more realistic and smooth (details of calculations are in lit fragment shader)
        float innerCutoff; // The inner cutoff angle of the spot light
        float outerCutoff; // The outer cutoff angle of the spot light

        // The ID of this component type is "Light"
        static std::string getID() { return "Light"; }

        // Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
    };

}