#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUp));
        
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, cameraUpDirection);
        //return glm::lookAt(cameraPosition, cameraTarget, this->cameraUpDirection);
    }

    void Camera::setPosition(const glm::vec3& position) {
        cameraPosition = position;
        // Update cameraFrontDirection based on the new position
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        // Update cameraRightDirection as well
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }

    void Camera::setTarget(const glm::vec3& target) {
        cameraTarget = target;
        // Update cameraFrontDirection based on the new target
        cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        // Update cameraRightDirection as well
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
    }


    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction) {
        case MOVE_FORWARD:
            cameraPosition += cameraFrontDirection * speed;
            break;
        case MOVE_BACKWARD:
            cameraPosition -= cameraFrontDirection * speed;
            break;
        case MOVE_LEFT:
            cameraPosition -= cameraRightDirection * speed;
            break;
        case MOVE_RIGHT:
            cameraPosition += cameraRightDirection * speed;
            cameraTarget = cameraPosition + cameraFrontDirection;
            break;
        case MOVE_UP:
            // Move camera up (e.g., along the world's up vector)
            cameraPosition += glm::vec3(0.0f, speed, 0.0f);
            break;
        case MOVE_DOWN:
            // Move camera down (e.g., along the world's down vector)
            cameraPosition -= glm::vec3(0.0f, speed, 0.0f);
            break;
        default:
            // Handle unknown direction or add more directions if needed
            break;
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
       // Adjust the pitch and yaw angles to radians
        float pitchRad = glm::radians(pitch);
        float yawRad = glm::radians(yaw);

        // Calculate the new front direction using spherical coordinates
        cameraFrontDirection = glm::normalize(glm::vec3(
            cos(pitchRad) * cos(yawRad),
            sin(pitchRad),
            cos(pitchRad) * sin(yawRad)
        ));

        // Recalculate the right and up directions based on the new front direction
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
        cameraUpDirection = glm::normalize(glm::cross(cameraRightDirection, cameraFrontDirection));
    }
}
