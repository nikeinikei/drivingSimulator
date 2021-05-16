#ifndef DRIVINGSIMULATOR_APPLICATION_H
#define DRIVINGSIMULATOR_APPLICATION_H

#include <stack>
#include <memory>

#include <GLFW/glfw3.h>

#include "GameState.h"

class Application {
public:
    explicit Application(GLFWwindow* window);

    ~Application() = default;

    void runMainLoop();

    void pushScene(const std::shared_ptr<GameState>& scene);

    void popScene();

    GLFWwindow* getWindow() const {
        return window;
    }

private:
    GLFWwindow* window;
    unsigned popScenes = 0;
    std::stack<std::shared_ptr<GameState>> scenes;
};

#endif //DRIVINGSIMULATOR_APPLICATION_H
