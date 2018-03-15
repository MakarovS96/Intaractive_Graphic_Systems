#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "libshader/Shader.h"
#include "camera/Camera.h"
#include "assimp_mloader/Model.h"
#include "assets.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libstbimage/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned char * fromHeight2Normal(unsigned char *textureData, int width, int height, int nrChannels, float scale = 3.0);
unsigned int load_texture(GLuint texture_num, char *texture_path, bool isHeight2Norm = false);
void animate();

int screenW = 800, screenH = 600;

glm::vec3 lightPos(0.0f, 1.0f, 2.0f);
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat lastX = screenW/2, lastY = screenH/2;
bool firstMouse = true;

float heightScale = 0.1f;

int main() {
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screenW, screenH,"Learn OpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
        return -1;
	}

	glViewport(0, 0, screenW, screenH);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);



    Shader shaderCylinder("shaders/cylinder.v.glsl", "shaders/cylinder.f.glsl");
    if(shaderCylinder.getID() == GL_FALSE) {
        glfwTerminate();
        return -1;
    }

    Shader lightShader("shaders/light.v.glsl", "shaders/light.f.glsl");
    if(lightShader.getID() == GL_FALSE) {
        glfwTerminate();
        return -1;
    }

    //stbi_set_flip_vertically_on_load(true);
    GLuint cylinder_tex = load_texture(0, "model/bricks2.jpg");

    GLuint cylinder_hight = load_texture(1, "model/bricks2_disp.jpg");

    GLuint cylinder_norm = load_texture(2, "model/bricks2_normal.jpg");

    //Main draw cicle
	while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)screenW / (GLfloat)screenH, 0.1f, 100.0f);

        glm::mat4 view;
        view = camera.GetViewMatrix();

        shaderCylinder.use();
        shaderCylinder.setMat4("view", view);
        shaderCylinder.setMat4("projection", projection);
        glm::mat4 cylinderModel;
        shaderCylinder.setMat4("model", cylinderModel);

        shaderCylinder.setVec3("lightPos", lightPos);
        shaderCylinder.setVec3("viewPos", camera.Position);

        shaderCylinder.setInt("diffuseMap", 0);
        shaderCylinder.setInt("depthMap", 1);
        shaderCylinder.setInt("normalMap", 2);

        shaderCylinder.setFloat("heightScale", heightScale);

        renderQuad();
        animate();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    screenW = width;
    screenH = height;
	glViewport(0, 0, screenW, screenH);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        if (heightScale > 0.0f)
            heightScale -= 0.0005f;
        else
            heightScale = 0.0f;
    } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (heightScale < 1.0f)
            heightScale += 0.0005f;
        else
            heightScale = 1.0f;
    }
}

void animate() {
    float angle = 0.4f * (float)glfwGetTime();
    lightPos.x = 3*cos ( angle );
    lightPos.y = 4*sin ( 1.4f * angle );
    lightPos.z = 4 + 0.5f * sin ( angle / 3 );
}

unsigned char * fromHeight2Normal(unsigned char *textureData, int width, int height, int nrChannels, float scale) {

    int length = width * height * 3;
    auto * result_tex = new unsigned char [length];
    for (int i = 0; i < length; i++) result_tex[i] = 0;
    float oneOver255 = 1.0f / 255.0f;

    int offs = 0;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            float	c  = textureData [i*width + j] * oneOver255;
            float	cx = textureData [i*width + (j+1)%width] * oneOver255;
            float	cy = textureData [((i+1)%height)*width + j] * oneOver255;

            float	dx = (c - cx) * scale;
            float	dy = (c - cy) * scale;

            float	len = sqrt ( dx*dx + dy*dy + 1 );

            // get normal
            float	nx = dy   / len;
            float	ny = -dx  / len;
            float	nz = 1.0f / len;

            // now convert to color and store in map
            result_tex [offs] = (unsigned char)(128 + 127*nx);
            result_tex [offs + 1] = (unsigned char)(128 + 127*ny);
            result_tex [offs + 2] = (unsigned char)(128 + 127*nz);

            offs += 3;
        }
    }
    return result_tex;
}

unsigned int load_texture(GLuint texture_num, char *texture_path, bool isHeight2Norm) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(texture_path, &width, &height, &nrComponents, 0);
    if (data)
    {
        if (isHeight2Norm) {
            data = fromHeight2Normal(data, width, height, nrComponents);
            nrComponents = 3;
        }

        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glActiveTexture(GL_TEXTURE0 + texture_num);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << texture_path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

