#ifndef DRIVINGSIMULATOR_SIMULATIONGAMESTATE_H
#define DRIVINGSIMULATOR_SIMULATIONGAMESTATE_H

#include <vector>
#include <future>
#include <optional>

#include "GameState.h"
#include "Scene.h"
#include "FreeCamera.h"

class SimulationGameState : public GameState {
public:
    explicit SimulationGameState(Application &application)
            : GameState(application) {
        camera = std::make_unique<FreeCamera>(application.getWindow());
        camera->attachControl();
        scene = std::make_unique<Scene>(*camera);
        obj = std::make_shared<Object>("resources/models/boden_mit_kegel.obj");
        scene->addObject(obj);
    }

    void draw() override {
        scene->draw();
    }

    void update(double dt) override {
        camera->update(dt);

        if (glfwGetKey(application.getWindow(), GLFW_KEY_P) == GLFW_PRESS) {
            if (!p_pressed) {
                p_pressed = true;
                auto inputMode = glfwGetInputMode(application.getWindow(), GLFW_CURSOR);
                if (inputMode == GLFW_CURSOR_NORMAL) {
                    camera->attachControl();
                } else {
                    camera->detachControl();
                }
            }
        }
        if (glfwGetKey(application.getWindow(), GLFW_KEY_P) == GLFW_RELEASE) {
            p_pressed = false;
        }
        if (glfwGetKey(application.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            application.popScene();
        }
    }

    void imgui() override {
        ImGui::Begin("performance metrics");

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);

        ImGui::End();
    }

private:
    bool p_pressed = false;
    bool first_frame = true;

    std::optional<std::future<void>> maybeFuture;

    std::unique_ptr<FreeCamera> camera;
    std::shared_ptr<Object> obj;
    std::unique_ptr<Scene> scene;
};

#endif //DRIVINGSIMULATOR_SIMULATIONGAMESTATE_H
