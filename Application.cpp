#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>

#include "Application.h"

Application::Application(GLFWwindow *window)
        : window(window) {
}

void Application::runMainLoop() {
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        if (scenes.empty())
            break;

        {
            auto &currentScene = scenes.top();
            glfwPollEvents();

            double currentTime = glfwGetTime();
            double dt = currentTime - lastTime;
            lastTime = currentTime;

            currentScene->update(dt);

            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            currentScene->draw();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            currentScene->imgui();

            // Rendering
            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        while (popScenes) {
            scenes.pop();
            popScenes--;
        }
    }
}

void Application::pushScene(const std::shared_ptr<GameState>& scene) {
    scenes.push(scene);
}

void Application::popScene() {
    popScenes++;
}
