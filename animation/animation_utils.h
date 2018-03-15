//
// Created by sennin on 3/16/18.
//

#ifndef HELLOTRIANGLE_ANIMATION_UTILS_H
#define HELLOTRIANGLE_ANIMATION_UTILS_H

#include <cstdlib>
#include <sys/time.h>
#include <map>
#include <vector>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

Assimp::Importer importer;
const aiScene *scene;
aiMesh *mesh;

aiMatrix4x4 globalInverseTransformation;

std::map<std::string, uint> boneMapping;
int numBones;

const int numMaxBones = 7;


struct VertexBoneDataN
{
    uint IDs[numMaxBones];
    float weights[numMaxBones];
};

struct VertexBoneDataNorm
{
    uint IDs[4];
    float weights[4];
};

struct BoneInfo
{
    aiMatrix4x4 boneOffset;
    aiMatrix4x4 finalTransformation;
};

std::vector<VertexBoneDataN> bones;
std::vector<VertexBoneDataNorm> bonesNormalize;
std::vector<BoneInfo> boneInfo;
std::vector<int> baseVertices;

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

GLuint vboVertex, vboNormal, vboUV;
GLuint vboSkeleton;
GLuint VAO = 0;

long long GetCurrentTimeMillis()
{
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
}

static long long startTime = GetCurrentTimeMillis();

float GetRunningTime()
{
    float runningTime = (float)((double)GetCurrentTimeMillis() - (double)startTime) / 1000.0;
    return runningTime;
}

void addBoneData(uint vertexID, uint boneID, float weight)
{
    int k;
    for(k = 0; k < ARRAY_SIZE_IN_ELEMENTS(bones[vertexID].IDs); k++)
    {
        if(bones[vertexID].weights[k] == 0.0)
        {
            bones[vertexID].IDs[k] = boneID;
            bones[vertexID].weights[k] = weight;
            return;
        }
    }
    assert(0);
}

// ñîðòèðîâêà ýëåìåíòîâ ñòðóêòóðû ïî óáûâàíèþ âåñîâ
void sortWeights(VertexBoneDataN &str)
{
    float tempWeight; int tempID;
    for(int i = numMaxBones - 1; i > 0; i--)
    {
        for(int j = 0; j < i; j++)
        {
            if(str.weights[j] < str.weights[j + 1])
            {
                tempWeight = str.weights[j];
                tempID = str.IDs[j];
                str.weights[j] = str.weights[j + 1];
                str.IDs[j] = str.IDs[j + 1];
                str.weights[j + 1] = tempWeight;
                str.IDs[j + 1] = tempID;
            }
        }
    }
}

/* ôóíêöèÿ äëÿ èçáàâëåíèÿ âåðøèí îò ëèøíèõ êîñòåé,
 * ò.å. îñòàâëÿåì íå áîëåå 4 êîñòåé íà âåðøèíó */
void normalizeWeights()
{
    for(int i = 0; i < bones.size(); i++)
    {
        // ñîðòèðóåì ïî óáûâàíèþ âåñîâ
        sortWeights(bones[i]);
        float k = 0;
        // ñ÷èòàåì ñóììó âåñîâ îñòàâøèõñÿ êîñòåé
        for(int j = 0; j < 4; j++)
            k += bones[i].weights[j];
        // ñ÷èòàåì êîýôôèöèåíò íîðìàëèçàöèè
        if(k != 0) k = 1.0 / k;
        // íîðìàëèçóåì âåñà (èõ ñóììà ðàâíà 1)
        for(int j = 0; j < 4; j++)
        {
            if(k != 0)
                bonesNormalize[i].weights[j] = bones[i].weights[j] * k;
            else bonesNormalize[i].weights[j] = 0.0;
            bonesNormalize[i].IDs[j] = bones[i].IDs[j];
        }
    }
}

bool load3DModel(const char* path)
{
    scene = importer.ReadFile(path, aiProcess_Triangulate);
    if(!scene)
    {
        std::cout << "3D Model not load by Assimp!";
        return false;
    }

    int numVertices = 0;
    baseVertices.resize(scene->mNumMeshes);

    // çàãðóçêà ñàìîé ìîäåëè
    for(int k = 0; k < scene->mNumMeshes; k++)
    {
        mesh = scene->mMeshes[k];
        baseVertices[k] = numVertices;
        numVertices += scene->mMeshes[k]->mNumVertices;

        for(int i = 0; i < mesh->mNumVertices; i++)
        {
            aiVector3D *pPos = &(mesh->mVertices[i]);
            aiVector3D *pNormal = &(mesh->mNormals[i]);
            aiVector3D *pTexCoord = &(mesh->mTextureCoords[0][i]);

            uvs.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
            normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
            vertices.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        }
    }

    globalInverseTransformation = scene->mRootNode->mTransformation;
    globalInverseTransformation.Inverse();

    numBones = 0;
    bones.resize(numVertices);
    bonesNormalize.resize(numVertices);

    // çàãðóçêà êîñòåé
    for(int k = 0; k < scene->mNumMeshes; k++)
    {
        mesh = scene->mMeshes[k];

        for(int i = 0; i < mesh->mNumBones; i++)
        {
            uint boneIndex = 0;
            std::string boneName(mesh->mBones[i]->mName.data);

            if(boneMapping.find(boneName) == boneMapping.end())
            {
                boneIndex = numBones;
                numBones++;
                BoneInfo bi;
                boneInfo.push_back(bi);
            }
            else boneIndex = boneMapping[boneName];

            boneInfo[boneIndex].boneOffset = mesh->mBones[i]->mOffsetMatrix;
            boneMapping[boneName] = boneIndex;

            int j;
            for(j = 0; j < mesh->mBones[i]->mNumWeights; j++)
            {
                uint vertexID = baseVertices[k] + mesh->mBones[i]->mWeights[j].mVertexId;
                float weight = mesh->mBones[i]->mWeights[j].mWeight;
                addBoneData(vertexID, boneIndex, weight);
            }
        }
    }

    normalizeWeights();

    return true;
}

// Ôóíêöèè ïîèñêà êëþ÷åâîé ïîçèöèè
int findPosition(float animationTime, aiNodeAnim *nodeAnim)
{
    for (int i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
    {
        if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}

// Ôóíêöèÿ ïîèñêà êëþ÷åâîãî âðàùåíèÿ
int findRotation(float animationTime, aiNodeAnim *nodeAnim)
{
    assert(nodeAnim->mNumRotationKeys > 0);

    for (int i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
    {
        if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}

// Ôóíêöèÿ ïîèñêà êëþ÷åâîãî ìàñøòàáèðîâàíèÿ
int findScaling(float animationTime, const aiNodeAnim *nodeAnim)
{
    assert(nodeAnim->mNumScalingKeys > 0);

    for (int i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
    {
        if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
        {
            return i;
        }
    }

    assert(0);
    return 0;
}

// Ôóíêöèÿ èíòåðïîëÿöèè ïîçèöèè
void calcInterpolatedPosition(aiVector3D &out, float animationTime, aiNodeAnim *nodeAnim)
{
    if (nodeAnim->mNumPositionKeys == 1)
    {
        out = nodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint positionIndex = findPosition(animationTime, nodeAnim);
    uint nextPositionIndex = (positionIndex + 1);
    assert(nextPositionIndex < nodeAnim->mNumPositionKeys);
    float deltaTime = (float)(nodeAnim->mPositionKeys[nextPositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
    float factor = (animationTime - (float)nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    aiVector3D &start = nodeAnim->mPositionKeys[positionIndex].mValue;
    aiVector3D &end = nodeAnim->mPositionKeys[nextPositionIndex].mValue;
    aiVector3D delta = end - start;
    out = start + factor * delta;
}

// Ôóíêöèÿ èíòåðïîëÿöèè êâàòåðíèîíà âðàùåíèÿ
void calcInterpolatedRotation(aiQuaternion &out, float animationTime, aiNodeAnim *nodeAnim)
{
    if (nodeAnim->mNumRotationKeys == 1)
    {
        out = nodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint rotationIndex = findRotation(animationTime, nodeAnim);
    uint nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < nodeAnim->mNumRotationKeys);
    float deltaTime = (float)(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
    float factor = (animationTime - (float)nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    aiQuaternion &startRotationQ = nodeAnim->mRotationKeys[rotationIndex].mValue;
    aiQuaternion &endRotationQ   = nodeAnim->mRotationKeys[nextRotationIndex].mValue;
    aiQuaternion::Interpolate(out, startRotationQ, endRotationQ, factor);
    out = out.Normalize();
}

// Ôóíêöèÿ èíòåðïîëÿöèè ìàñøòàáèðîâàíèÿ
void calcInterpolatedScaling(aiVector3D &out, float animationTime, const aiNodeAnim *nodeAnim)
{
    if (nodeAnim->mNumScalingKeys == 1)
    {
        out = nodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint scalingIndex = findScaling(animationTime, nodeAnim);
    uint nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < nodeAnim->mNumScalingKeys);
    float deltaTime = (float)(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
    float factor = (animationTime - (float)nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    aiVector3D &start = nodeAnim->mScalingKeys[scalingIndex].mValue;
    aiVector3D &end   = nodeAnim->mScalingKeys[nextScalingIndex].mValue;
    aiVector3D delta = end - start;
    out = start + factor * delta;
}

// Ôóíêöèÿ ïîèñêà íóæíîãî êàíàëà àíèìàöèè
aiNodeAnim* findNodeAnim(aiAnimation* animation, std::string nodeName)
{
    for (int i = 0; i < animation->mNumChannels; i++)
    {
        aiNodeAnim *nodeAnim = animation->mChannels[i];

        if(std::string(nodeAnim->mNodeName.data) == nodeName)
        {
            return nodeAnim;
        }
    }
    return NULL;
}

void initScaleTransform(aiMatrix4x4 &m, float ScaleX, float ScaleY, float ScaleZ)
{
    m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
    m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
    m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
    m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;
}

void initTranslationTransform(aiMatrix4x4 &m, float x, float y, float z)
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

void initIdentity(aiMatrix4x4 &m)
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}

// Ôóíêöèÿ îáõîäèò ëèñòû äåðåâà è ãåíåðèðóåò èòîãîâîå ïðåîáðàçîâàíèå äëÿ êàæäîãî ëèñòà
void readNodeHeirarchy(float animationTime, aiNode* node, aiMatrix4x4 &parentTransform)
{
    std::string nodeName(node->mName.data);
    aiAnimation *animation = scene->mAnimations[0];
    aiMatrix4x4 nodeTransformation(node->mTransformation);
    aiNodeAnim *nodeAnim = findNodeAnim(animation, nodeName);

    if (nodeAnim)
    {
        aiVector3D scaling;
        calcInterpolatedScaling(scaling, animationTime, nodeAnim);
        aiMatrix4x4 scalingM;
        initScaleTransform(scalingM, scaling.x, scaling.y, scaling.z);

        aiQuaternion rotationQ;
        calcInterpolatedRotation(rotationQ, animationTime, nodeAnim);
        aiMatrix4x4 rotationM = aiMatrix4x4(rotationQ.GetMatrix());

        aiVector3D translation;
        calcInterpolatedPosition(translation, animationTime, nodeAnim);
        aiMatrix4x4 translationM;
        initTranslationTransform(translationM, translation.x, translation.y, translation.z);

        nodeTransformation = translationM * rotationM * scalingM;
    }

    aiMatrix4x4 globalTransformation = parentTransform * nodeTransformation;

    if (boneMapping.find(nodeName) != boneMapping.end())
    {
        uint boneIndex = boneMapping[nodeName];
        boneInfo[boneIndex].finalTransformation = globalInverseTransformation * globalTransformation * boneInfo[boneIndex].boneOffset;
    }

    for (uint i = 0; i < node->mNumChildren; i++)
    {
        readNodeHeirarchy(animationTime, node->mChildren[i], globalTransformation);
    }
}

// Ôóíêöèÿ ïðåîáðàçîâàíèÿ ñêåëåòà êàæäûé êàäð
void boneTransform(float timeInSeconds, std::vector<aiMatrix4x4> &transforms)
{
    aiMatrix4x4 identity;
    initIdentity(identity);

    float ticksPerSecond = (float)(scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0);
    float timeInTicks = timeInSeconds * ticksPerSecond;
    float animationTime = fmod(timeInTicks, (float)scene->mAnimations[0]->mDuration);

    readNodeHeirarchy(animationTime, scene->mRootNode, identity);
    transforms.resize(numBones);

    for (int i = 0; i < numBones; i++)
    {
        transforms[i] = boneInfo[i].finalTransformation;
    }
}

void setBoneTransform(GLuint id, aiMatrix4x4 transform)
{
    glUniformMatrix4fv(id, 1, GL_TRUE, &transform[0][0]);
}

void render() {
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &vboVertex);
        glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glGenBuffers(1, &vboNormal);
        glBindBuffer(GL_ARRAY_BUFFER, vboNormal);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glGenBuffers(1, &vboUV);
        glBindBuffer(GL_ARRAY_BUFFER, vboUV);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

        glGenBuffers(1, &vboSkeleton);
        glBindBuffer(GL_ARRAY_BUFFER, vboSkeleton);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bonesNormalize[0]) * bonesNormalize.size(), &bonesNormalize[0],
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_UNSIGNED_INT, GL_FALSE, sizeof(VertexBoneDataNorm), (void*)0);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneDataNorm),
                              (void*)offsetof(VertexBoneDataNorm, weights));
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

#endif //HELLOTRIANGLE_ANIMATION_UTILS_H
