//
// Created by sennin on 14.03.18.
//

#ifndef HELLOTRIANGLE_MODEL_H
#define HELLOTRIANGLE_MODEL_H

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <iostream>

class Model
{
public:
    /*  Методы   */
    Model(char *path)
    {
        loadModel(path);
    }
    void Draw();
private:
    /*  Данные модели  */
    vector<Mesh> meshes;
    string directory;
    /*  Методы   */
    void loadModel(string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
};


#endif //HELLOTRIANGLE_MODEL_H
