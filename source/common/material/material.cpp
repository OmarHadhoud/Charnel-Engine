#include "material.hpp"

#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our {

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const {
        //TODO: Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;

        if(data.contains("pipelineState")){
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
        lit = data.value("lit", false);
        ui = data.value("ui", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint 
    void TintedMaterial::setup() const {
        //TODO: Write this function
        Material::setup();
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex" 
    void TexturedMaterial::setup() const {
        //TODO: Write this function
        TintedMaterial::setup();
        // Set alpathreshold uniform
        shader->set("alphaThreshold", alphaThreshold);

        // make texture unit 0 active and bind the texture to it
        glActiveTexture(GL_TEXTURE0);
        texture->bind();
        // bind the sampler to texture unit 0
        sampler->bind(0);
        // send the unit number as a uniform for the shader
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json& data){
        TintedMaterial::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    // This function should call the setup of its parent and
    // set the different texture maps we have and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold and
    // set the shininess uniform to the value in the member variable shininess and
    // set the material tints we have
    // Then it should bind the textures and sampler to corresponding texture units and send the unit numbers to the uniform variables
    void LitMaterial::setup() const {
        Material::setup();
        // Set alpathreshold uniform
        shader->set("material.alphaThreshold", alphaThreshold);
        // set the shininess
        shader->set("material.shininess", shininess);
        // set the tints
        shader->set("material.albedo_tint", albedo_tint);
        shader->set("material.specular_tint", specular_tint);
        shader->set("material.emission_tint", emission_tint);
        // set the roughness range
        shader->set("material.roughness_range", roughness_range);

        // make texture unit 0 active and bind the texture to it
        glActiveTexture(GL_TEXTURE0);
        albedo->bind();
        // bind the sampler to texture unit 0
        sampler->bind(0);

        // add if condition to prevent runtime error, but behaviour is undefined (will use previously bounded textures)
        // if we have a specular map provided, bind it to texture unit 1
        if (specular)
        {
            // make texture unit 1 active and bind the texture to it
            glActiveTexture(GL_TEXTURE1);
            specular->bind();
            // bind the sampler to texture unit 1
            sampler->bind(1);
        }

        // if we have a roughness map provided, bind it to texture unit 2
        if(roughness)
        {
            // make texture unit 2 active and bind the texture to it
            glActiveTexture(GL_TEXTURE2);
            roughness->bind();
            // bind the sampler to texture unit 2
            sampler->bind(2);
        }

        // if we have an ambient_occlusion map provided, bind it to texture unit 3
        if (ambient_occlusion)
        {
            // make texture unit 3 active and bind the texture to it
            glActiveTexture(GL_TEXTURE3);
            ambient_occlusion->bind();
            // bind the sampler to texture unit 3
            sampler->bind(3);
        }

        // if we have an emission map provided, bind it to texture unit 4
        if (emission)
        {
            // make texture unit 4 active and bind the texture to it
            glActiveTexture(GL_TEXTURE4);
            emission->bind();
            // bind the sampler to texture unit 4
            sampler->bind(4);
        }

        // send the unit numbers as a uniform for the shader
        shader->set("material.albedo", 0);
        shader->set("material.specular", 1);
        shader->set("material.roughness", 2);
        shader->set("material.ambient_occlusion", 3);
        shader->set("material.emission", 4);

    }

    // This function read the material data from a json object
    void LitMaterial::deserialize(const nlohmann::json& data){
        Material::deserialize(data);
        if(!data.is_object()) return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        shininess = data.value("shininess", 0.0f);
        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        albedo_tint = data.value("albedo_tint", glm::vec3(1.0f));
        specular = AssetLoader<Texture2D>::get(data.value("specular", "default_specular"));
        specular_tint = data.value("specular_tint", glm::vec3(1.0f));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", "default_roughness"));
        roughness_range = data.value("roughness_range", glm::vec2(1.0f));
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", "default_a_o"));
        emission = AssetLoader<Texture2D>::get(data.value("emission", "default_emission"));
        emission_tint = data.value("emission_tint", glm::vec3(1.0f));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
        lit = true;
    }

}