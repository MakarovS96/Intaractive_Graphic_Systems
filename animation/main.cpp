#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "libshader/Shader.h"
#include "camera/Camera.h"
#include "assimp_mloader/Model.h"
#include "animation_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libstbimage/stb_image.h"

#define CUBEMAP_SIZE 800
#define CUBEMAP_OFFSET 0


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned char * fromHeight2Normal(unsigned char *textureData, int width, int height, int nrChannels, float scale = 3.0);
void load_texture(GLuint & texture, GLuint texture_num ,const char* texture_path, bool isHeightMap = false, GLuint color_model = GL_RGB);
void animate();
void InitCubeTexture(GLuint &cubeMap);
void CreateCubeTexture(GLuint cubeMap, Shader &shaderTerx, Shader &shaderCylinder, Model &cylinder, Shader &lightShader, Model &lightSphere);

int screenW = 800, screenH = 800;

glm::vec3 lightPos(0.0f, 1.0f, 2.0f);
glm::vec3 cubePos(3.0f, 2.0f, -2.0f);
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

GLfloat lastX = screenW/2, lastY = screenH/2;
bool firstMouse = true;

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
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glDepthFunc(GL_LEQUAL);

    Shader shaderCylinder("shaders/cylinder.v.glsl", "shaders/cylinder.f.glsl");
    Shader shaderTerx("shaders/trex.v.glsl", "shaders/trex.f.glsl");
    Shader lightShader("shaders/light.v.glsl", "shaders/light.f.glsl");

    if(shaderTerx.getID() == GL_FALSE) {
        glfwTerminate();
        return -1;
    }

    if(shaderCylinder.getID() == GL_FALSE) {
        glfwTerminate();
        return -1;
    }

    if(lightShader.getID() == GL_FALSE) {
        glfwTerminate();
        return -1;
    }

    Shader shaderRefract("shaders/refract.v.glsl", "shaders/refract.f.glsl");
    if(shaderRefract.getID() == GL_FALSE) {
        glfwTerminate();
        return -1;
    }

    Model lightSphere("model/sphere.obj");
    Model cylinder("model/cylinder.obj");
    //Model trex("model/trex.x");
    load3DModel("model/trex.x");
    Model refractCube("model/sphere.obj");


    //stbi_set_flip_vertically_on_load(true);
    GLuint cylinder_tex;
    load_texture(cylinder_tex, 0, "model/rockwall.tga");

    GLuint cylinder_normal;
    load_texture(cylinder_normal, 1, "model/rockwall_height.tga", true);

    GLuint trex_tex;
    load_texture(trex_tex, 2, "model/trex_texture.jpg");

    GLuint cube_tex;
    InitCubeTexture(cube_tex);

    //Main draw cicle
	while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        CreateCubeTexture(cube_tex, shaderTerx, shaderCylinder, cylinder, lightShader, lightSphere);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(camera.Zoom), (GLfloat)screenW / (GLfloat)screenH, 0.1f, 100.0f);

        glm::mat4 view;
        view = camera.GetViewMatrix();

        shaderTerx.use();
        shaderTerx.setMat4("view", view);
        shaderTerx.setMat4("projection", projection);

        glm::mat4 trexModel;
        trexModel = glm::translate(trexModel, glm::vec3(3.0f, 0.0f, 3.0f));
        trexModel = glm::scale(trexModel, glm::vec3(0.4f));
        shaderTerx.setMat4("model", trexModel);

        shaderTerx.setVec3("viewPos", camera.Position);
        shaderTerx.setVec3("lightPos", lightPos);
        shaderTerx.setInt("textureSampler",2);

        std::vector<aiMatrix4x4> transforms;
        float runningTime = GetRunningTime();
        boneTransform(runningTime, transforms);

        for(int i = 0; i < transforms.size(); i++)
        {
            char name[128];
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "bones[%d]", i);
            GLuint idTransform = glGetUniformLocation(shaderTerx.getID(), name);
            setBoneTransform(idTransform, transforms[i]);
        }

        render();

        shaderCylinder.use();
        shaderCylinder.setMat4("view", view);
        shaderCylinder.setMat4("projection", projection);
        glm::mat4 cylinderModel;
        shaderCylinder.setMat4("model", cylinderModel);
        shaderCylinder.setVec3("lightPos", lightPos);
        shaderCylinder.setVec3("viewPos", camera.Position);
        shaderCylinder.setInt("diffuseMap", 0);
        shaderCylinder.setInt("normalMap", 1);
        cylinder.Draw();

        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glm::mat4 lightModel;
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f));
        lightShader.setMat4("model", lightModel);
        lightSphere.Draw();

        glEnable(GL_TEXTURE_CUBE_MAP);
        shaderRefract.use();
        shaderRefract.setMat4("view", view);
        shaderRefract.setMat4("projection", projection);
        glm::mat4 refractModel;
        refractModel = glm::translate(refractModel, cubePos);
        shaderRefract.setMat4("model", refractModel);
        shaderRefract.setVec3("viewPos", camera.Position);
        shaderRefract.setInt("cubeMap", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cube_tex);
        refractCube.Draw();
        glDisable(GL_TEXTURE_CUBE_MAP);

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
}

void animate() {
    float angle = 0.4f * (float)glfwGetTime();
    lightPos.x = 3*cos ( angle );
    lightPos.y = 4*sin ( 1.4f * angle );
    lightPos.z = 4 + 0.5f * sin ( angle / 3 );
}

void load_texture(GLuint & texture, GLuint texture_num ,const char* texture_path, bool isHeightMap, GLuint color_model) {
    glGenTextures(1,&texture);
    glActiveTexture(GL_TEXTURE0 + texture_num);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *textureData = stbi_load(texture_path,&width,&height,&nrChannels, 0);

    if (textureData == nullptr) {
        std::cout << "Image \""<< texture_path <<"\" not founded." << std::endl ;
        glfwTerminate();
    }

    if (isHeightMap) textureData = fromHeight2Normal(textureData, width, height, nrChannels);

    glTexImage2D(GL_TEXTURE_2D, 0, color_model, width, height, 0, color_model, GL_UNSIGNED_BYTE, textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(textureData);
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
            result_tex [offs    ] = (unsigned char)(128 + 127*nx);
            result_tex [offs + 1] = (unsigned char)(128 + 127*ny);
            result_tex [offs + 2] = (unsigned char)(128 + 127*nz);

            offs += 3;
        }
    }
    return result_tex;
}

void InitCubeTexture(GLuint &cubeMap)
{
    glEnable(GL_TEXTURE_CUBE_MAP);
    glGenTextures(1, &cubeMap);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

    for ( int i = 0; i < 6; i++ )
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    glDisable (GL_TEXTURE_CUBE_MAP);
}

void CreateCubeTexture(GLuint cubeMap, Shader &shaderTerx, Shader &shaderCylinder, Model &cylinder, Shader &lightShader, Model &lightSphere)
{
    //glViewport(CUBEMAP_OFFSET, CUBEMAP_OFFSET, CUBEMAP_SIZE, CUBEMAP_SIZE);

    for (int i = 0; i < 6; i++)
    {
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 view;

        switch(i)
        {
            case 0: {
                view = glm::lookAt(cubePos, cubePos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                break;
            }

            case 1: {
                view = glm::lookAt(cubePos, cubePos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                break;
            }

            case 2: {
                view = glm::lookAt(cubePos, cubePos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                break;
            }

            case 3: {
                view = glm::lookAt(cubePos, cubePos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
                break;
            }

            case 4: {
                view = glm::lookAt(cubePos, cubePos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                break;
            }

            case 5: {
                view = glm::lookAt(cubePos, cubePos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                break;
            }
        }

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(90.0f), (float)screenW/screenH, 0.1f, 1000.0f);


        shaderTerx.use();
        shaderTerx.setMat4("view", view);
        shaderTerx.setMat4("projection", projection);
        glm::mat4 trexModel;
        trexModel = glm::translate(trexModel, glm::vec3(3.0f, 0.0f, 3.0f));
        trexModel = glm::scale(trexModel, glm::vec3(0.4f));
        shaderTerx.setMat4("model", trexModel);

        shaderTerx.setVec3("viewPos", camera.Position);
        shaderTerx.setVec3("light.position", lightPos);
        shaderTerx.setVec3("light.ambient", glm::vec3(0.1f));
        shaderTerx.setVec3("light.diffuse", glm::vec3(0.5f));
        shaderTerx.setVec3("light.specular", glm::vec3(1.0f));
        shaderTerx.setVec3("material.specular", glm::vec3(0.5f));
        shaderTerx.setFloat("material.shininess",64);
        shaderTerx.setInt("material.diffuse",2);

        shaderCylinder.use();
        shaderCylinder.setMat4("view", view);
        shaderCylinder.setMat4("projection", projection);
        glm::mat4 cylinderModel;
        shaderCylinder.setMat4("model", cylinderModel);
        shaderCylinder.setVec3("lightPos", lightPos);
        shaderCylinder.setVec3("viewPos", camera.Position);
        shaderCylinder.setInt("diffuseMap", 0);
        shaderCylinder.setInt("normalMap", 1);
        cylinder.Draw();



        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glm::mat4 lightModel;
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.1f));
        lightShader.setMat4("model", lightModel);
        lightSphere.Draw();

        glEnable(GL_TEXTURE_CUBE_MAP);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
        glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, 0, 0, screenW, screenH);
        glDisable(GL_TEXTURE_CUBE_MAP);
    }

    glEnable(GL_TEXTURE_2D);
    //glViewport(0, 0, screenW, screenH);
}