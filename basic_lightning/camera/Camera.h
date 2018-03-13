//
// Created by sennin on 12.03.18.
//
#pragma once

#ifndef HELLOTRIANGLE_CAMERA_H
#define HELLOTRIANGLE_CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  3.0f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;

class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;

        GLfloat Yaw;
        GLfloat Pitch;

        GLfloat MovementSpeed;
        GLfloat MouseSensitivity;
        GLfloat Zoom;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) :
                Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
        {
            this->Position = position;
            this->WorldUp = up;
            this->Yaw = yaw;
            this->Pitch = pitch;
            this->updateCameraVectors();
        }

        Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
                Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVTY), Zoom(ZOOM)
        {
            this->Position = glm::vec3(posX, posY, posZ);
            this->WorldUp = glm::vec3(upX, upY, upZ);
            this->Yaw = yaw;
            this->Pitch = pitch;
            this->updateCameraVectors();
        }

        glm::mat4 GetViewMatrix();
        void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime);
        void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
        void ProcessMouseScroll(GLfloat yoffset);

private:
    void updateCameraVectors();

};


#endif //HELLOTRIANGLE_CAMERA_H
