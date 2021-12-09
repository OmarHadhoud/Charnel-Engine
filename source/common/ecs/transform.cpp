#include "entity.hpp"
#include "../deserialize-utils.hpp"

#include <glm/gtx/euler_angles.hpp>

namespace our {

    // This function computes and returns a matrix that represents this transform
    // Remember that the order of transformations is: Scaling, Rotation then Translation
    // HINT: to convert euler angles to a rotation matrix, you can use glm::yawPitchRoll
    glm::mat4 Transform::toMat4() const {
        //TODO: Write this function

        // note: the order of operations is scaling, rotation, translation: new_pos = translation * rotation * scaling * original_pos
        // but here we are doing translation, rotation, scaling in glm, as glm does the operations in the following way:
        // if we do glm::scale(translation_matrix, scale)
        // it calculates the scale matrix first, then multiplies it with the translation matrix: return translation_matrix * scale_matrix
        // which is the required, i.e.: read glm operations from down to up


        // create an identity matrix to use it for the operations of the transform
        glm::mat4 mat = glm::mat4(1.0f);
        // translate this identity matrix to the position of the transform
        mat = glm::translate(mat, this->position);
        // calculate the rotation matrix from the euler angles
        glm::mat4 rot = glm::yawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z);
        // update the transformation matrix by multiplying it with the rotation matrix
        mat = mat * rot;
        // finally, scale the matrix by the scale of the transform
        mat = glm::scale(mat, this->scale);
        return mat;
    }

     // Deserializes the entity data and components from a json object
    void Transform::deserialize(const nlohmann::json& data){
        position = data.value("position", position);
        rotation = glm::radians(data.value("rotation", glm::degrees(rotation)));
        scale    = data.value("scale", scale);
    }

}