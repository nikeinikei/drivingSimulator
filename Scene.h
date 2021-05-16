#ifndef DRIVINGSIMULATOR_SCENE_H
#define DRIVINGSIMULATOR_SCENE_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "AbstractCamera.h"


class Object {
public:
    explicit Object(const char *path);

    ~Object();

    [[nodiscard]] glm::mat4 getModelProjection() const;

    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei numIndices;

    friend class Scene;
};

class Scene {
public:
    explicit Scene(const AbstractCamera &camera);

    ~Scene();

    void addObject(const std::shared_ptr<Object>& obj);

    void draw();

    const AbstractCamera &camera;

private:
    GLuint program;
    GLuint vertexShader;
    GLuint fragmentShader;

    GLint camUniformLocation;
    GLint modelUniformLocation;
    GLint lightColorLocation;
    GLint lightPositionLocation;
    GLint viewPositionLocation;

    std::vector<std::shared_ptr<Object>> objects;
};

#endif //DRIVINGSIMULATOR_SCENE_H
