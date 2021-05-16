#include <GLFW/glfw3.h>
#include "FreeCamera.h"
#include <glm/gtc/matrix_transform.hpp>

FreeCamera::FreeCamera(GLFWwindow *window)
        : window(window) {
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
}

glm::mat4 FreeCamera::getView() const {
    return glm::lookAt(
            position,
            position + front,
            up
    );
}

void FreeCamera::update(double dt) {
    if (!paused) {

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        const float sensitivity = 0.1;

        auto dx = (float) (mouseX - lastMouseX);
        auto dy = (float) (mouseY - lastMouseY);

        yaw += dx * sensitivity;
        pitch -= dy * sensitivity;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);

        lastMouseX = mouseX;
        lastMouseY = mouseY;

        const float cameraSpeed = 5.0f * (float) dt;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += cameraSpeed * front;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            position -= cameraSpeed * front;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            position -= glm::normalize(glm::cross(front, up)) * cameraSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            position += glm::normalize(glm::cross(front, up)) * cameraSpeed;
        }
    }
}

glm::vec3 FreeCamera::getPosition() const {
    return position;
}

void FreeCamera::detachControl() {
    paused = true;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void FreeCamera::attachControl() {
    paused = false;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &lastMouseX, &lastMouseY);

}
