#ifndef DRIVINGSIMULATOR_ABSTRACTCAMERA_H
#define DRIVINGSIMULATOR_ABSTRACTCAMERA_H

#include <glm/glm.hpp>

class AbstractCamera {
public:
    [[nodiscard]] virtual glm::mat4 getView() const = 0;
    [[nodiscard]] virtual glm::vec3 getPosition() const = 0;
};

#endif //DRIVINGSIMULATOR_ABSTRACTCAMERA_H
