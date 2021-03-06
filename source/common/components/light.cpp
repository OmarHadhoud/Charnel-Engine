#include "light.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    // Reads light parameters from the given json object
    void LightComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        std::string lightTypeStr = data.value("lightType", "Direct");
        if(lightTypeStr == "Direct")
        {
            lightType = LightType::DIRECT;
        } else if(lightTypeStr == "Point")
        {
            lightType = LightType::POINT;
        }
        else
        {
            lightType = LightType::SPOT;
        }
        diffuse = data.value("diffuse", diffuse);
        specular = data.value("specular", specular);
        ambient = data.value("ambient", ambient);
        attenuationConstant = data.value("attenuationConstant", 1.0f);
        attenuationLinear = data.value("attenuationLinear", 0.0f);
        attenuationQuadratic = data.value("attenuationQuadratic", 0.0f);
        innerCutoff = glm::radians(data.value("innerCutoff", 0.0f));
        outerCutoff = glm::radians(data.value("outerCutoff", 0.0f));
    }
}