#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"
#include "Libs/stb_image.h"

const GLint WIDTH = 800, HEIGHT = 600;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

//choose model and texture here
//if add or remove, go to modelPosition array and to the same
struct Model {
    std::string name;
    std::string texture;
    int shaderIndex;

    Model(const std::string& n, const std::string& tex, int shader)
        : name(n), texture(tex), shaderIndex(shader) {}
};

std::vector<Model> models = {
        {"cube", "uvmap", 1},
        {"table", "table", 0},
        {"bigcake", "bigcake", 0},
        {"balloon", "red", 0},
        {"balloon", "red", 0},
        {"balloon", "red", 0},
        {"balloon", "red", 0},
        {"balloon", "red", 0},
        {"balloon", "red", 0},
        {"balloon", "red", 0},
        {"balloon", "red", 0},
        {"floor", "oakfloor", 0},
        {"wall", "wall", 0}
    };

std::vector<std::string> vShaders = {"shader", "lightShader"};
std::vector<std::string> fShaders = {"shader", "lightShader"};

float yaw = -90.0f, pitch = 0.0f;

glm::vec3 lightColour = glm::vec3(255.0f, 255.0f, 255.0f);
glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);

void CreateOBJ() {
    for (int i = 0; i < models.size(); i++){
        Mesh *obj = new Mesh();
        bool loaded = obj->CreateMeshFromOBJ(("Models/" + models[i].name + ".obj").c_str());
        if (loaded){
            meshList.push_back(obj);
        }
        else{
            std::cout<<"Failed to load model"<<std::endl;
        }
    }
}

void CreateShaders()
{
    for (int i = 0; i < vShaders.size(); i++){
        std::string frag = "Shaders/" + fShaders[i] + ".frag";
        std::string vert = "Shaders/" + vShaders[i] + ".vert";
        Shader* shader = new Shader();
        shader->CreateFromFiles(vert.c_str(), frag.c_str());
        shaderList.push_back(shader);
    }
}

void CreateTextures(unsigned int texture[])
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (int i = 0; i < models.size(); i++){
        glGenTextures(1, &texture[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);

        int width, height, nrChannels;
        unsigned char *data = 0;

        stbi_set_flip_vertically_on_load(true);
        data = stbi_load(("Textures/" + models[i].texture + ".png").c_str(), &width, &height, &nrChannels,0);

        if (data){
            //bind image with texture
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else{
            std::cout<<"Failed to load texture"<<std::endl;
        }

        stbi_image_free(data);
    }
}

void checkMouse() {
    // Get window size
    int width, height;
    glfwGetWindowSize(mainWindow.getWindow(), &width, &height);

    // Get center coordinates
    double centerX = width / 2.0;
    double centerY = height / 2.0;

    // Get current cursor position
    double xpos, ypos;
    glfwGetCursorPos(mainWindow.getWindow(), &xpos, &ypos);

    // Calculate cursor movement offsets
    double xoffset = xpos - centerX;
    double yoffset = centerY - ypos; // Invert y-axis

    // Set sensitivity
    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Update yaw and pitch
    yaw += xoffset;
    pitch -= yoffset;

    // Clamp pitch to prevent flipping
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    // Reset cursor position to the center
    glfwSetCursorPos(mainWindow.getWindow(), centerX, centerY);
}

void checkKeyboard(glm::vec3 &cameraPos, const glm::vec3 &cameraDirection, const glm::vec3 &cameraRight, const glm::vec3 &up, const float deltaTime)
{
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_W) == GLFW_PRESS){
        cameraPos += cameraDirection * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_S) == GLFW_PRESS){
        cameraPos -= cameraDirection * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_A) == GLFW_PRESS){
        cameraPos -= cameraRight * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_D) == GLFW_PRESS){
        cameraPos += cameraRight * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(mainWindow.getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        cameraPos -= up * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS){
        cameraPos += up * deltaTime * 5.0f;
    }

    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_UP) == GLFW_PRESS){
        lightPos += cameraDirection * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS){
        lightPos -= cameraDirection * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_LEFT) == GLFW_PRESS){
        lightPos -= cameraRight * deltaTime * 5.0f;
    }
    if(glfwGetKey(mainWindow.getWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS){
        lightPos += cameraRight * deltaTime * 5.0f;
    }
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateOBJ();
    CreateShaders();

    lightColour /= 255;
    GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

    glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 4.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPos);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
    //glm::mat4 projection = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.1f, 100.0f);

    unsigned int texture[models.size()];
    CreateTextures(texture);

    float deltaTime, lastFrame;

    //Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glm::mat4 view (1.0f);

        //Get + Handle user input events
        glfwPollEvents();

        checkMouse();
        std::cout << "Cam Pos: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")" << std::endl;

        glm::vec3 direction;

        direction.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        direction.y = sin(glm::radians(pitch));

        checkKeyboard(cameraPos, cameraDirection, cameraRight, up, deltaTime);

        //lock
        //cameraPos.y = 5;

        cameraDirection = glm::normalize(direction);
        cameraRight = glm::normalize(glm::cross(cameraDirection, up));
        cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

        //Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        view = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);

        //draw here
        glm::vec3 modelPositions[] =
        {
            lightPos,//cubelight
            glm::vec3(0.0f, 0.0f, 0.0f),//table
            glm::vec3(0.0f, 0.80f, 0.0f),//cake
            glm::vec3(1.0f, 0.85f, 0.0f),//balloon
            glm::vec3(1.0f, 0.95f, -0.2f),//balloon
            glm::vec3(1.3f, 0.85f, 0.1f),//balloon
            glm::vec3(1.1f, 0.75f, 0.0f),//balloon
            glm::vec3(1.0f, 0.85f, 0.0f),//balloon
            glm::vec3(-1.2f, 0.85f, 0.3f),//balloon
            glm::vec3(1.3f, 0.85f, 0.0f),//balloon
            glm::vec3(1.1f, 0.85f, 0.0f),//balloon
            glm::vec3(0.0f, 0.0f, 0.0f),//floor
            glm::vec3(0.0f, 0.0f, 0.0f),//wall
        };

        glm::vec3 modelScale[] =
        {
            glm::vec3(0.2f, 0.2f, 0.2f),//cubelight
            glm::vec3(1.0f, 1.0f, 1.0f),//table
            glm::vec3(0.5f, 0.5f, 0.5f),//cake
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//balloon
            glm::vec3(1.0f, 1.0f, 1.0f),//floor
            glm::vec3(1.0f, 1.0f, 1.0f),//wall
        };
        //Object
        for (int i = 0; i < models.size(); i++)
        {
            int shaderIndex = models[i].shaderIndex;
            shaderList[shaderIndex]->UseShader();
            uniformModel = shaderList[shaderIndex]->GetUniformLocation("model");
            uniformProjection = shaderList[shaderIndex]->GetUniformLocation("projection");
            uniformView = shaderList[shaderIndex]->GetUniformLocation("view");

            glm::mat4 model (1.0f);

            model = glm::translate(model, modelPositions[i]);
            //model = glm::rotate(model, glm::radians(2.0f * i) ,glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::scale(model, modelScale[i]);

            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
            //light
            glUniform3fv(shaderList[shaderIndex]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
            glUniform3fv(shaderList[shaderIndex]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);
            glUniform3fv(shaderList[shaderIndex]->GetUniformLocation("viewPos"), 1, (GLfloat *)&cameraPos);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            meshList[i]->RenderMesh();
        }
        
        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}
