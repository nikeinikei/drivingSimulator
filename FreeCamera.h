#ifndef DRIVINGSIMULATOR_FREECAMERA_H
#define DRIVINGSIMULATOR_FREECAMERA_H

#include <glm/glm.hpp>

#include "AbstractCamera.h"

class FreeCamera : public AbstractCamera {

public:
    explicit FreeCamera(GLFWwindow* window);

    [[nodiscard]] glm::mat4 getView() const override;

    [[nodiscard]] glm::vec3 getPosition() const override;

    void update(double dt);

    void detachControl();

    void attachControl();

private:
    bool paused = true;
    GLFWwindow* window;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 4.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    double lastMouseX;
    double lastMouseY;

    float yaw = -90.0f;
    float pitch = 0.0f;
};


#endif //DRIVINGSIMULATOR_FREECAMERA_H
