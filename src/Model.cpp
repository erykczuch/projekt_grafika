#include "Model.h"
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

Model::Model(const std::string& modelName, const glm::vec3& position)
{
    this->position = position;
    this->modelMatrix = glm::translate(glm::mat4(1.0f), position);
    
    LoadModel(modelName);
    SetupMesh();
}

Model::~Model()
{
}

void Model::Destroy()
{
    if (vao) {
        vao->Delete();
        delete vao;
        vao = nullptr;
    }
    if (vbo) {
        vbo->Delete();
        delete vbo;
        vbo = nullptr;
    }
}

void Model::LoadModel(const std::string& modelName)
{
    std::string inputfile = "../assets/models/" + modelName + ".obj";
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "../assets/textures/" + modelName + "/";

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;

        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            size_t fv = shape.mesh.num_face_vertices[f];
            std::vector<glm::vec3> facePos(fv);

            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                facePos[v] = glm::vec3(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
            }

            glm::vec3 normal(0.0f);
            if (fv >= 3)
            {
                glm::vec3 a = facePos[0];
                glm::vec3 b = facePos[1];
                glm::vec3 c = facePos[2];
                normal = glm::normalize(glm::cross(b - a, c - a));
            }

            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                glm::vec3 pos = facePos[v];
                glm::vec2 uv(0.0f);

                if (idx.texcoord_index >= 0)
                {
                    uv = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                }

                // position
                vertices.push_back(pos.x);
                vertices.push_back(pos.y);
                vertices.push_back(pos.z);
                // color
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
                // uv
                vertices.push_back(uv.x);
                vertices.push_back(uv.y);
                // normal
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);

                indices.push_back(static_cast<unsigned int>(index_offset + v));
            }
            index_offset += fv;
        }
    }

    for (const auto& mat : materials)
    {
        std::cout << "Diffuse texture: " << mat.diffuse_texname << std::endl;

        if (!mat.diffuse_texname.empty())
        {
            Texture tex(
                ("../assets/textures/" + modelName + "/" + mat.diffuse_texname).c_str(),
                GL_TEXTURE_2D,
                GL_TEXTURE0,
                GL_RGBA,
                GL_UNSIGNED_BYTE
            );

            diffuseTextures.push_back(std::move(tex));
        }
    }
}

void Model::SetupMesh()
{
    vao = new VAO();
    vao->Bind();

    vbo = new VBO(vertices.data(), vertices.size() * sizeof(float));

    vao->LinkAttrib(*vbo, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    vao->LinkAttrib(*vbo, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    vao->LinkAttrib(*vbo, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    vao->LinkAttrib(*vbo, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

    vao->Unbind();
    vbo->Unbind();
}

void Model::DrawDepth(Shader& shadowMapShader)
{
    glUniformMatrix4fv(glGetUniformLocation(shadowMapShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    vao->Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 11);
    vao->Unbind();
}

void Model::Draw(Shader& shader, Camera& camera, GLuint depthMap)
{
    shader.Activate();

    // Update model matrix from current position
    modelMatrix = glm::translate(glm::mat4(1.0f), position);

    glUniformMatrix4fv(
        glGetUniformLocation(shader.ID, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(modelMatrix)
    );

    if (!diffuseTextures.empty())
    {
        glActiveTexture(GL_TEXTURE0);
        diffuseTextures[0].Bind();
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    vao->Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 11);
    vao->Unbind();
}
