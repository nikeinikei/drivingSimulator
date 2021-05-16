#include <iostream>
#include <string>
#include <vector>

#include "tiny_obj_loader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scene.h"


const GLchar *vertexSource = R"glsl(
#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;

uniform mat4 proj;
uniform mat4 cam;
uniform mat4 model;

out vec3 p_color;
out vec2 p_texCoord;
out vec3 p_normal;
out vec3 p_fragPos;

void main() {
    gl_Position = proj * cam * model * vec4(position, 1.0);

    p_color = color;
    p_texCoord = texCoord;
    p_normal = normal;
    p_fragPos = vec3(model * vec4(position, 1.0));
}

)glsl";

const GLchar *fragmentSource = R"glsl(
#version 400 core

in vec3 p_color;
in vec2 p_texCoord;
in vec3 p_normal;
in vec3 p_fragPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 outColor;

void main() {
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(p_normal);
    vec3 lightDir = normalize(lightPos - p_fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - p_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * p_color;
    outColor = vec4(result, 1.0);
}
)glsl";

static GLuint createShader(GLenum type, const GLchar *source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        std::cout << "shader not compiled successfully" << std::endl;
    }
    return shader;
}

Scene::Scene(const AbstractCamera &camera)
        : camera(camera) {
    vertexShader = createShader(GL_VERTEX_SHADER, vertexSource);
    fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentSource);
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glBindFragDataLocation(program, 0, "outColor");
    glLinkProgram(program);
    glUseProgram(program);

    GLint projectionUniformLocation = glGetUniformLocation(program, "proj");
    camUniformLocation = glGetUniformLocation(program, "cam");
    modelUniformLocation = glGetUniformLocation(program, "model");
    lightColorLocation = glGetUniformLocation(program, "lightColor");
    lightPositionLocation = glGetUniformLocation(program, "lightPos");
    viewPositionLocation = glGetUniformLocation(program, "viewPos");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1600.0f / 900.0f, 0.1f, 1000.0f);
    glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, glm::value_ptr(projection));

    float whiteColor[3] = {0.2f, 0.8f, 1.0f};
    glUniform3fv(lightColorLocation, 1, whiteColor);

    float lightPosition[3] = {-3.0f, 2.0f, 5.0f};
    glUniform3fv(lightPositionLocation, 1, lightPosition);
}

Scene::~Scene() {
    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Scene::draw() {
    glUseProgram(program);

    glUniform3fv(viewPositionLocation, 1, glm::value_ptr(camera.getPosition()));

    for (const auto &obj : objects) {
        glBindVertexArray(obj->vao);

        glUniformMatrix4fv(camUniformLocation, 1, GL_FALSE, glm::value_ptr(camera.getView()));
        glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, glm::value_ptr(obj->getModelProjection()));

        glDrawElements(GL_TRIANGLES, obj->numIndices, GL_UNSIGNED_INT, nullptr);
    }
}

void Scene::addObject(const std::shared_ptr<Object> &obj) {
    objects.push_back(obj);
}

Object::Object(const char *path) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn;
    std::string err;
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, "resources/materials");

    if (!warn.empty()) {
        std::cout << "warning when loading obj: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cout << "error when loading obj: " << err << std::endl;
    }

    if (!ret) {
        std::cout << "failed to load/parse .obj" << std::endl;
    }

    std::vector<GLuint> indices;
    std::vector<GLfloat> vertexData;

    for (auto &shape : shapes) {
        unsigned long i = 0;
        for (auto index : shape.mesh.indices) {
            auto vertexIndex = index.vertex_index;
            auto normalIndex = index.normal_index;
            auto texCoordIndex = index.texcoord_index;
            vertexData.push_back(attrib.vertices[3 * vertexIndex]);
            vertexData.push_back(attrib.vertices[3 * vertexIndex + 1]);
            vertexData.push_back(attrib.vertices[3 * vertexIndex + 2]);
            vertexData.push_back(attrib.colors[3 * vertexIndex]);
            vertexData.push_back(attrib.colors[3 * vertexIndex + 1]);
            vertexData.push_back(attrib.colors[3 * vertexIndex + 2]);
            vertexData.push_back(attrib.texcoords[2 * texCoordIndex]);
            vertexData.push_back(attrib.texcoords[2 * texCoordIndex + 1]);
            vertexData.push_back(attrib.normals[3 * normalIndex]);
            vertexData.push_back(attrib.normals[3 * normalIndex + 1]);
            vertexData.push_back(attrib.normals[3 * normalIndex + 2]);

            indices.push_back(i++);
        }
    }

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *) (8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    numIndices = (GLsizei) indices.size();
    glBindVertexArray(0);
}

Object::~Object() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

glm::mat4 Object::getModelProjection() const {
    return glm::translate(glm::mat4(1.0f), position);
}
