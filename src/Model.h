#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <vector>
#include <string>

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "VAO.h"
#include "VBO.h"
#include "Texture.h"
#include "shaderClass.h"
#include "Camera.h"

class Model {
public:
    Model(const std::string& modelName, const glm::vec3& position = glm::vec3(0.0f));
    ~Model();

    void Draw(Shader& shader, Camera& camera, GLuint depthMap);
    void DrawDepth(Shader& shadowMapShader);
    void Destroy();

    glm::vec3 position;
    glm::mat4 modelMatrix;

private:
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    VAO* vao;
    VBO* vbo;
    std::vector<Texture> diffuseTextures;

    void LoadModel(const std::string& modelName);
    void SetupMesh();
};

#endif