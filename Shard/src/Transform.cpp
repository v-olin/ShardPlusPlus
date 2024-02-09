#include "Transform.h"

//#include <math.h>
#include <cmath>

#include "gtc/matrix_transform.hpp"

// this should be included from math.h but isnt for some reason
//#define M_PI 3.14159265358979323846264338327950288

namespace Shard {

    Transform::Transform()
        : transformMatrix(glm::mat4{ 1.f })
        , lastTransformMatrix(glm::mat4{ 1.f })
        , forward(glm::vec3{ 1.f,0,0 })
        , right(glm::vec3{ 0,1.f,0 })
        , up(glm::vec3{ 0,0,1.f })
    {
        rotate(0, forward);
    }

    Transform::Transform(std::shared_ptr<Transform> src) {
        // TODO: do in initializerl ist        
        transformMatrix = src->transformMatrix;
        lastTransformMatrix = src->lastTransformMatrix;
        forward = src->forward;
        right = src->right;
        up = src->up;

        // Deep copy for sprite_path
        if (!src->sprite_path.empty()) {
            // TODO: is this a deep copy hmm??
            sprite_path = src->sprite_path;
        }
    }

    // these two are stolen from
    // https://www.reddit.com/r/opengl/comments/sih6lc/4x4_matrix_to_position_rotation_and_scale/
    glm::vec3 Transform::position() {
        return glm::vec3(transformMatrix[3]);
    }

    glm::vec3 Transform::size() {
        return glm::vec3{
            glm::length(glm::vec3(transformMatrix[0])),
            glm::length(glm::vec3(transformMatrix[1])),
            glm::length(glm::vec3(transformMatrix[2]))
        };
    }


    glm::vec3 Transform::scale() {
        glm::vec3 col1{ transformMatrix[0][0], transformMatrix[0][1], transformMatrix[0][2] };
        glm::vec3 col2{ transformMatrix[1][0], transformMatrix[1][1], transformMatrix[1][2] };
        glm::vec3 col3{ transformMatrix[2][0], transformMatrix[2][1], transformMatrix[2][2] };
        return { glm::length(col1), glm::length(col2), glm::length(col3) };
    }

    glm::vec3 Transform::rotation() {
        // get rotation matrix from model matrix:
        // https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati

        // this should cut off 4th column and 4th row
        glm::mat3 rotMatrix = glm::mat3(getRotationMatrix());

        /*
        get euler angles from rotation matrix
		https://stackoverflow.com/questions/1996957/conversion-euler-to-matrix-and-matrix-to-euler

        rotMatrix = Ry(H)*Rx(P)*Rz(B)
            = | cos H*cos B+sin H*sin P*sin B  cos B*sin H*sin P-sin B*cos H  cos P*sin H |
              |                   cos P*sin B                    cos B*cos P       -sin P |
              | sin B*cos H*sin P-sin H*cos B  sin H*sin B+cos B*cos H*sin P  cos P*cos H |
        
        */
        
        // x = [1], y = [2], z = [3]
        // x = P = arcsin(-rot[3][2])
        float x = asin(-rotMatrix[2].y);
        // y = H = arctan(rot[3][1]/rot[3][3])
        float y = atan2(rotMatrix[2].x, rotMatrix[2].z);
        // z = B = arctan(rot[1][2]/rot[2][2])
        float z = atan2(rotMatrix[0].y, rotMatrix[1].y);

        return { x, y, z };
    }

    void Transform::translate(const glm::vec3& force) {
        lastTransformMatrix = transformMatrix;
        transformMatrix = glm::translate(transformMatrix, force);
    }

	// get top left 3x3 matix and scale it with the basis vectors
	// such that all values are [-1, 1] (in trig form)
    glm::mat3 Transform::getRotationMatrix() {
        auto scale = this->scale();
        return glm::mat3(
            glm::vec3(transformMatrix[0] / scale.x),
            glm::vec3(transformMatrix[1] / scale.y),
            glm::vec3(transformMatrix[2] / scale.z)
        );
    }

    void Transform::rotate(const float angle_deg, const glm::vec3& axis) {
        transformMatrix = glm::rotate(transformMatrix, angle_deg, axis);
        glm::mat3 rot = getRotationMatrix();

        // we also need to rotate the axis vectors of the transform
        forward = glm::normalize(rot * glm::vec3{ 1.f, 0, 0});
        right = glm::normalize(rot * glm::vec3{ 0, 1.f, 0 });
        up = glm::normalize(rot * glm::vec3{ 0, 0, 1.f });
    }

    glm::vec3 Transform::getLastDirection() {
        glm::vec3 pos = glm::vec3(transformMatrix[3]);
        glm::vec3 lastPos = glm::vec3(lastTransformMatrix[3]);
        return lastPos - pos;
    }

    void Transform::scale(const glm::vec3& scale) {
        transformMatrix = glm::scale(transformMatrix, scale);
    }

}