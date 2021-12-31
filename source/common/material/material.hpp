#pragma once

#include "pipeline-state.hpp"
#include "../texture/texture2d.hpp"
#include "../texture/sampler.hpp"
#include "../shader/shader.hpp"

#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our {

    // This is the base class for all the materials
    // It contains the 3 essential components required by any material
    // 1- The pipeline state when drawing objects using this material
    // 2- The shader program used to draw objects using this material
    // 3- Whether this material is transparent or not
    // Materials that send uniforms to the shader should inherit from the is material and add the required uniforms
    class Material {
    public:
        PipelineState pipelineState;
        ShaderProgram* shader;
        bool transparent;
        bool lit;

        // This function does 2 things: setup the pipeline state and set the shader program to be used
        virtual void setup() const;
        // This function read a material from a json object
        virtual void deserialize(const nlohmann::json& data);
    };

    // This material adds a uniform for a tint (a color that will be sent to the shader)
    // An example where this material can be used is when the whole object has only color which defined by tint
    class TintedMaterial : public Material {
    public:
        glm::vec4 tint;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };

    // This material adds two uniforms (besides the tint from Tinted Material)
    // The uniforms are:
    // - "tex" which is a Sampler2D. "texture" and "sampler" will be bound to it.
    // - "alphaThreshold" which defined the alpha limit below which the pixel should be discarded
    // An example where this material can be used is when the object has a texture
    class TexturedMaterial : public TintedMaterial {
    public:
        Texture2D* texture;
        Sampler* sampler;
        float alphaThreshold;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };

    // This material adds needed texture maps for the lit material that will be used when we have lighting
    // note: we are deriving from Material not TexturedMaterial as we don't want to have the texture variable, as
    // we will create multiple textures here with better naming
    class LitMaterial : public Material {
    public:
        // the albedo/diffuse texture aand its color tint
        Texture2D* albedo;
        glm::vec3 albedo_tint;
        // the specular/glossy texture and its color tint
        Texture2D* specular;
        glm::vec3 specular_tint;
        // the roughness texture and the range of the values for it
        // as the texture values range is from 0 to 1, we need to map it using roughnes_range
        Texture2D* roughness;
        glm::vec2 roughness_range;
        // the ambient occlusion texture, which shows how much the object has ambient light
        Texture2D* ambient_occlusion;
        // the emission texture, which is a constant color whether there's light or not (makes an emission effect, like in lava)
        // and the tint of it
        Texture2D* emission;
        glm::vec3 emission_tint;
        // the sampler that we will use with this material
        Sampler* sampler;

        // the alpha threshold for the material that will be used for transparency
        float alphaThreshold;
        // the shininess of the material, it will be used instead of the roughness texture if it is larger than 0.1f
        // (in case we don't have a roughness texture)
        float shininess;

        void setup() const override;
        void deserialize(const nlohmann::json& data) override;
    };

    // This function returns a new material instance based on the given type
    inline Material* createMaterialFromType(const std::string& type){
        if(type == "tinted"){
            return new TintedMaterial();
        } else if(type == "textured"){
            return new TexturedMaterial();
        } else if(type == "lit") {
            return new LitMaterial();
        } else {
            return new Material();
        }
    }

}