#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/light.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>

namespace our
{
    
    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand {
        glm::mat4 localToWorld;
        glm::vec3 center;
        Mesh* mesh;
        Material* material;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer {
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;
        std::vector<RenderCommand> uiCommands;
    public:
        // This function should be called every frame before rendering to setup the lights in the scene
        void setupLights(World* world, ShaderProgram* shaderProgram) {
            // use the shader program before setting the lights uniforms
            shaderProgram->use();
            int lightCount = 0;
            // loop over all the entities in the world and check if they have a light component
            for (auto entity : world->getEntities()) {
                // Get the light component if exists
                auto light = entity->getComponent<LightComponent>();
                if (light == nullptr)
                    continue;
                // if we get thee light component, we set the light uniforms

                // light direction is always forward like the camera, but transform it to world space
                auto light_dir = glm::vec3((entity->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0)));
                // set the uniforms for this light in the shader program
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].type", int(light->lightType));
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].position", entity->localTransform.position);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].direction", light_dir);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].diffuse", light->diffuse);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].specular", light->specular);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].ambient", light->ambient);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].attenuationConstant", light->attenuationConstant);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].attenuationLinear", light->attenuationLinear);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].attenuationQuadratic", light->attenuationQuadratic);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].innerCutoff", light->innerCutoff);
                shaderProgram->set("lights[" + std::to_string(lightCount) + "].outerCutoff", light->outerCutoff);
                lightCount++;
            }
            // send the number of lights to the shader program as we need it for the fragment shader
            shaderProgram->set("lightCount", lightCount);
        }

        // This function should be called every frame to draw the given world
        // Both viewportStart and viewportSize are using to define the area on the screen where we will draw the scene
        // viewportStart is the lower left corner of the viewport (in pixels)
        // viewportSize is the width & height of the viewport (in pixels). It is also used to compute the aspect ratio
        void render(World* world, glm::ivec2 viewportStart, glm::ivec2 viewportSize){
            // First of all, we search for a camera and for all the mesh renderers
            // we also search for the lit shader if it is used in a draw command, so that we use it to setup lights later
            CameraComponent* camera = nullptr;
            CameraComponent* cameraMinimap = nullptr;
            ShaderProgram* litShader = nullptr;
            opaqueCommands.clear();
            transparentCommands.clear();
            uiCommands.clear();
            for(auto entity : world->getEntities()){
                // If we hadn't found a camera yet, we look for a camera in this entity
                CameraComponent* cameraComp = entity->getComponent<CameraComponent>();
                if (cameraComp != nullptr && cameraComp->minimap == false)
                    camera = cameraComp;
                if (cameraComp != nullptr && cameraComp->minimap == true)
                    cameraMinimap = cameraComp;
                // If this entity has a mesh renderer component
                if(auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer){
                    if (!meshRenderer->enabled)
                        continue;
                    // We construct a command from it
                    RenderCommand command;
                    command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                    command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                    command.mesh = meshRenderer->mesh;
                    command.material = meshRenderer->material;
                    // If the material is lit, we get the lit shader
                    if (command.material->lit)
                        litShader = meshRenderer->material->shader;
                    // If the material is ui, we add it to ui commands
                    if (command.material->ui)
                        uiCommands.push_back(command);
                    // if it is transparent, we add it to the transparent commands list
                    else if(command.material->transparent){
                        transparentCommands.push_back(command);
                    } else {
                    // Otherwise, we add it to the opaque command list
                        opaqueCommands.push_back(command);
                    }
                }
            }

            // if we use lights in the scene, then setup it for once before drawing
            if(litShader)
                setupLights(world, litShader);

            // If there is no camera, we return (we cannot render without a camera)
            if(camera == nullptr) return;

            //TODO: Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
            // HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
            glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0);
            std::sort(transparentCommands.begin(), transparentCommands.end(), [cameraForward](const RenderCommand& first, const RenderCommand& second){
                //TODO: Finish this function
                // HINT: the following return should return true "first" should be drawn before "second".
                // create a vector from first object to second object
                auto first_to_second = second.center - first.center;
                // get the dot product of the vector and the camera forward vector which will indicate if the first_to_second is in same direction
                // of the forward, if it isn't, then the first object is further than the second and should be drawn first
                auto dot_prod = glm::dot(first_to_second, cameraForward);
                return dot_prod < 0;
            });

            //TODO: Get the camera ViewProjection matrix and store it in VP
            auto VP = camera->getProjectionMatrix(viewportSize) * camera->getViewMatrix();
            //TODO: Set the OpenGL viewport using viewportStart and viewportSize
            glViewport(viewportStart.x, viewportStart.y, viewportSize.x, viewportSize.y);
            //TODO: Set the clear color to black and the clear depth to 1
            glClearColor(0, 0, 0, 1);
            glClearDepth(1);
            //TODO: Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            glDepthMask(GL_TRUE);
            //TODO: Clear the color and depth buffers
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //TODO: Draw all the opaque commands followed by all the transparent commands
            // Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
            // notice that we send more matrices that are needed for lighting like the normal transformation matrix
            // and the viewproj matrix, also we need to send the camera position in world space
            for (auto command : opaqueCommands)
            {
                command.material->setup();
                command.material->shader->set("transform", VP * command.localToWorld);
                command.material->shader->set("model", command.localToWorld);
                command.material->shader->set("model_inv_transpose", glm::transpose(glm::inverse(command.localToWorld)));
                command.material->shader->set("view_proj", VP);
                command.material->shader->set("camera_pos", glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1)));
                command.mesh->draw();
            }

            for (auto command : transparentCommands)
            {
                command.material->setup();
                command.material->shader->set("transform", VP * command.localToWorld);
                command.material->shader->set("model", command.localToWorld);
                command.material->shader->set("model_inv_transpose", glm::transpose(glm::inverse(command.localToWorld)));
                command.material->shader->set("view_proj", VP);
                command.material->shader->set("camera_pos", glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1)));
                command.mesh->draw();
            }

            for (auto command : uiCommands)
            {
                command.material->setup();
                command.material->shader->set("transform", VP * command.localToWorld);
                command.material->shader->set("model", command.localToWorld);
                command.material->shader->set("model_inv_transpose", glm::transpose(glm::inverse(command.localToWorld)));
                command.material->shader->set("view_proj", VP);
                command.material->shader->set("camera_pos", glm::vec3(camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1)));
                command.mesh->draw();
            }

            if (cameraMinimap)
            {
                VP = cameraMinimap->getProjectionMatrix(viewportSize) * cameraMinimap->getViewMatrix();
                glViewport(viewportSize.x - 400, viewportSize.y -400, 400, 400);
                glClearDepth(1);
                glDepthMask(GL_TRUE);
                glClear(GL_DEPTH_BUFFER_BIT);
                for (auto command : opaqueCommands)
                {
                    command.material->setup();
                    command.material->shader->set("transform", VP * command.localToWorld);
                    command.material->shader->set("model", command.localToWorld);
                    command.material->shader->set("model_inv_transpose", glm::transpose(glm::inverse(command.localToWorld)));
                    command.material->shader->set("view_proj", VP);
                    command.material->shader->set("camera_pos", glm::vec3(cameraMinimap->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1)));
                    command.mesh->draw();
                }

                for (auto command : transparentCommands)
                {
                    command.material->setup();
                    command.material->shader->set("transform", VP * command.localToWorld);
                    command.material->shader->set("model", command.localToWorld);
                    command.material->shader->set("model_inv_transpose", glm::transpose(glm::inverse(command.localToWorld)));
                    command.material->shader->set("view_proj", VP);
                    command.material->shader->set("camera_pos", glm::vec3(cameraMinimap->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1)));
                    command.mesh->draw();
                }
            }
        };

    };

}