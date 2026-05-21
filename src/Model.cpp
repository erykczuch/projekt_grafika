#include "Model.h"
#include <iostream>
#include <fstream>
#include <sstream>

// enabling TinyObjLoader implementation
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

// initializing transform values, material settings, loading the model data
Model::Model(const std::string& modelName, const glm::vec3& position, const glm::vec3& scale)
{
    this->position = position;
    this->scale = scale;
	this->rotation = glm::vec3(0.0f);
    
    this->shininess = 32.0f;
    this->specularStrength = 0.6f;

    this->modelMatrix = glm::translate(glm::mat4(1.0f), position);
    this->modelMatrix = glm::scale(modelMatrix, scale);

    // load mesh data from OBJ file
    LoadModel(modelName);

    // create VAO/VBO and configure vertex attributes
    SetupMesh();
}

Model::~Model()
{
}

// frees GPU resources and prevents dangling pointers
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

// changes texture filtering settings
void Model::SetTextureFiltering(GLenum minFilter, GLenum magFilter)
{
    for (auto& tex : diffuseTextures)
    {
        tex.SetFiltering(minFilter, magFilter);
    }
}

void Model::SetBlurStrength(float strength)
{
    // the higher the strength the blurrier it gets, by sampling smaller mipmaps
    for (auto& tex : diffuseTextures)
    {
        tex.SetBlur(strength);
    }
}

// loads geometry and material data from OBJ and MTL files
void Model::LoadModel(const std::string& modelName)
{
    std::string inputfile = "../assets/models/" + modelName + ".obj";
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "../assets/textures/" + modelName + "/";

    tinyobj::ObjReader reader;

    // analyzing OBJ file
    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    // printing any warnings
    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // process each mesh shape
    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;

        // iterate through all faces in the mesh
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

                // read texture coordinates from OBJ file
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

                // color (white)
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);
                vertices.push_back(1.0f);

                // texture coordinates
                vertices.push_back(uv.x);
                vertices.push_back(uv.y);

                // normal
                vertices.push_back(normal.x);
                vertices.push_back(normal.y);
                vertices.push_back(normal.z);

                // store vertex index
                indices.push_back(static_cast<unsigned int>(index_offset + v));
            }
            // move offset to the next face
            index_offset += fv;
        }
    }

    // loading additional data from MTL file
    std::string mtlPath = "../assets/textures/" + modelName + "/" + modelName + ".mtl";
    std::ifstream mtlFile(mtlPath);
    if (mtlFile.is_open())
    {
        std::string line;
        while (std::getline(mtlFile, line))
        {
            std::istringstream iss(line);
            std::string token;
            if (iss >> token)
            {
                // diffuse map texture
                if (token == "map_Kd")
                {
                    std::string texName;
                    if (iss >> texName)
                    {
                        std::cout << "Diffuse texture (from " << modelName << ".mtl): " << texName << std::endl;
                        Texture tex(
                            ("../assets/textures/" + modelName + "/" + texName).c_str(),
                            GL_TEXTURE_2D,
                            GL_TEXTURE0,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE
                        );

                        diffuseTextures.push_back(std::move(tex));
                    }
                }
                // shininess intensity
                else if (token == "Ns")
                {
                    iss >> this->shininess;
                }
                //  specular reflection color and intensity
                else if (token == "Ks")
                {
                    float r, g, b;
                    if (iss >> r >> g >> b)
                    {
                        this->specularStrength = (r + g + b) / 3.0f;
                    }
                }
            }
        }
    }
}

// creates VAO/VBO and configures vertex attribute layout
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

// renders the model into the shadow depth map
void Model::DrawDepth(Shader& shadowMapShader)
{
    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    modelMatrix = glm::scale(modelMatrix, scale);

    glUniformMatrix4fv(
        glGetUniformLocation(shadowMapShader.ID, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(modelMatrix)
    );

    vao->Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 11);
    vao->Unbind();
}

// main rendering with textures, lighting, and shadows
void Model::Draw(Shader& shader, Camera& camera, GLuint depthMap)
{
    shader.Activate();

    // update model about position, rotation and scale
    modelMatrix = glm::mat4(1.0f);

    modelMatrix = glm::translate(modelMatrix, position);

    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

    modelMatrix = glm::scale(modelMatrix, scale);

    // upload model matrix to shader
    glUniformMatrix4fv(
        glGetUniformLocation(shader.ID, "model"),
        1,
        GL_FALSE,
        glm::value_ptr(modelMatrix)
    );

    glUniform1f(glGetUniformLocation(shader.ID, "shininess"), shininess);
    glUniform1f(glGetUniformLocation(shader.ID, "specularStrength"), specularStrength);

    if (!diffuseTextures.empty())
    {
        glActiveTexture(GL_TEXTURE0);
        diffuseTextures[0].Bind();
    }

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    // draw model geometry
    vao->Bind();
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 11);
    vao->Unbind();
}
