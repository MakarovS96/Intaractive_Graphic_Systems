//
// Created by sennin on 14.03.18.
//
#pragma once
#ifndef HELLOTRIANGLE_MESH_H
#define HELLOTRIANGLE_MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices);

    void Draw();

private:
    unsigned int VAO, VBO, EBO;
    void setupMesh();
};


#endif //HELLOTRIANGLE_MESH_H
