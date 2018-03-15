#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../libstbimage/stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "../libshader/Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }


    void Draw()
    {
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw();
    }

private:
    void loadModel(string const &path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        directory = path.substr(0, path.find_last_of('/'));


        processNode(scene->mRootNode, scene);
    }

    void processNode(aiNode *node, const aiScene *scene)
    {

        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {

            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene)
    {

        vector<Vertex> vertices;
        vector<unsigned int> indices;

        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector;

            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;

            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;

            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;

                vector.x = mesh->mTangents[i].x;
                vector.y = mesh->mTangents[i].y;
                vector.z = mesh->mTangents[i].z;
                vertex.Tangent = vector;

                vector.x = mesh->mBitangents[i].x;
                vector.y = mesh->mBitangents[i].y;
                vector.z = mesh->mBitangents[i].z;
                vertex.Bitangent = vector;
            } else {
                vertex.TexCoords = glm::vec2(0.0f);
                vertex.Tangent = glm::vec3(0.0f);
                vertex.Bitangent = glm::vec3(0.0f);
            }


            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }


        return Mesh(vertices, indices);
    }

};

bool loadOBJ(const char* path, bool triangles, std::vector<glm::vec3> &out_vertices, std::vector<glm::vec2> &out_uvs, std::vector<glm::vec3> &out_normals)
{
    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    FILE *file = fopen(path, "r");
    if(file == NULL)
    {
        cout << "Error parsing" << endl;
        return false;
    }

    while(true)
    {
        char lineHeader[128];
        int res = fscanf(file, "%s", lineHeader);
        if(res == EOF) break;
        else
        {
            if(strcmp(lineHeader, "v") == 0)
            {
                glm::vec3 vertex;
                char x[50], y[50], z[50];
                fscanf(file, "%s %s %s\n", x, y, z);
                vertex.x = atof(x);
                vertex.y = atof(y);
                vertex.z = atof(z);
                temp_vertices.push_back(vertex);
            }
            else if(strcmp(lineHeader, "vt") == 0)
            {
                glm::vec2 uv;
                char x[50], y[50];
                fscanf(file, "%s %s\n", x, y);
                uv.x = atof(x);
                uv.y = atof(y);
                temp_uvs.push_back(uv);
            }
            else if(strcmp(lineHeader, "vn") == 0)
            {
                glm::vec3 normal;
                char x[50], y[50], z[50];
                fscanf(file, "%s %s %s\n", x, y, z);
                normal.x = atof(x);
                normal.y = atof(y);
                normal.z = atof(z);
                temp_normals.push_back(normal);
            }
            else if(strcmp(lineHeader, "f") == 0)
            {
                if(triangles == true)
                {
                    unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                    int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
                    if(matches != 9)
                    {
                        cout << "Error parsing" << endl;
                        return false;
                    }
                    vertexIndices.push_back(vertexIndex[0]);
                    vertexIndices.push_back(vertexIndex[1]);
                    vertexIndices.push_back(vertexIndex[2]);
                    uvIndices.push_back(uvIndex[0]);
                    uvIndices.push_back(uvIndex[1]);
                    uvIndices.push_back(uvIndex[2]);
                    normalIndices.push_back(normalIndex[0]);
                    normalIndices.push_back(normalIndex[1]);
                    normalIndices.push_back(normalIndex[2]);
                }

                else
                {
                    unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];
                    int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
                    if(!(matches == 9 || matches == 12))
                    {
                        cout << "Error parsing" << endl;
                        return false;
                    }
                    int k;
                    if(matches == 9) k = 2;
                    else k = 3;
                    vertexIndices.push_back(vertexIndex[0]);
                    vertexIndices.push_back(vertexIndex[1]);
                    vertexIndices.push_back(vertexIndex[2]);
                    vertexIndices.push_back(vertexIndex[k]);
                    uvIndices.push_back(uvIndex[0]);
                    uvIndices.push_back(uvIndex[1]);
                    uvIndices.push_back(uvIndex[2]);
                    uvIndices.push_back(uvIndex[k]);
                    normalIndices.push_back(normalIndex[0]);
                    normalIndices.push_back(normalIndex[1]);
                    normalIndices.push_back(normalIndex[2]);
                    normalIndices.push_back(normalIndex[k]);
                }
            }
        }
    }

    for(unsigned int i = 0; i < vertexIndices.size(); i++)
    {

        glm::vec3 vertex = temp_vertices[vertexIndices[i] - 1];
        glm::vec2 uv = temp_uvs[uvIndices[i] - 1];
        glm::vec3 normal = temp_normals[normalIndices[i] - 1];

        out_vertices.push_back(vertex);
        out_uvs.push_back(uv);
        out_normals.push_back(normal);
    }

    return true;
}

void CalcTB(std::vector<glm::vec3> &vertices, std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &tangentses, std::vector<glm::vec3> &bitangentses)
{
    for(int i = 0; i < vertices.size(); i+=3)
    {
        glm::vec3 deltaPos1 = vertices[i + 1] - vertices[i + 0];
        glm::vec3 deltaPos2 = vertices[i + 2] - vertices[i + 0];
        glm::vec2 deltaUV1 = uvs[i + 1] - uvs[i + 0];
        glm::vec2 deltaUV2 = uvs[i + 2] - uvs[i + 0];

        float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = r * (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
        glm::vec3 bitangent = r * (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x);

        tangentses.push_back(tangent);
        tangentses.push_back(tangent);
        tangentses.push_back(tangent);

        bitangentses.push_back(bitangent);
        bitangentses.push_back(bitangent);
        bitangentses.push_back(bitangent);
    }
}
#endif
