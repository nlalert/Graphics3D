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
Shader* depthShader;

struct Model {
    std::string name;
    std::string texture;
    int shaderIndex;
    float diffuseStrength;
    float specularStrength;
    float shininess;

    Model(const std::string& n, const std::string& tex, int shader, float diffuse, float specular, float shiny)
        : name(n), texture(tex), shaderIndex(shader), diffuseStrength(diffuse), specularStrength(specular), shininess(shiny) {}
};
//choose model and texture here
//if add or remove, go to modelPosition array and to the same

std::vector<Model> models = {
        // obj, texture, shader, diffuse, spec, shiny
        //diffuseLight != 0
        {"cube", "uvmap", 1, 0, 0, 0},
        {"table", "table", 0, 0.5f, 0.1f, 1.0f},
        {"bigcakenoflame", "bigcakenoflame", 0, 0.5f, 0.1f, 1.0f},
        {"balloon", "yellow", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "yellow", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "yellow", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "yellow", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "blue", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "blue", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "blue", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "blue", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "red", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "red", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "red", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "red", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "red", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "green", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "green", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "green", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "green", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "green", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "pink", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "pink", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "pink", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "pink", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "pink", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "orange", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "orange", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "orange", 0, 0.5f, 0.5f, 256.0f},
        {"balloon", "orange", 0, 0.5f, 0.5f, 256.0f},
        {"floor", "oakfloor", 0, 0.5f, 0.5f, 256.0f},
        {"wall", "wall", 0, 0.5f, 0.1f, 1.0f},
        {"banner", "banner", 0, 0.5f, 0.1f, 1.0f},
        {"hat", "hat", 0, 0.5f, 0.1f, 1.0f},
        {"plate", "plate", 0, 0.5f, 0.5f, 512.0f},
        {"drawer", "white", 0, 0.5f, 0.1f, 1.0f},
        {"TV", "TV", 0, 0.3f, 0.8f, 256.0f},
        {"bunting", "bunting", 0, 0.5f, 0.1f, 1.0f},
        {"bunting", "bunting", 0, 0.5f, 0.1f, 1.0f},
        {"bunting", "bunting", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"confetti", "confetti", 0, 0.5f, 0.1f, 1.0f},
        {"woodtable", "woodtable", 0, 0.5f, 0.1f, 1.0f},
        {"gift", "gift", 0, 0.5f, 0.1f, 1.0f},
    };

std::vector<std::string> vShaders = {"shader", "lightShader"};
std::vector<std::string> fShaders = {"shader", "lightShader"};

float yaw = -90.0f, pitch = 0.0f;
GLuint uniformModel = 0, uniformProjection = 0, uniformView = 0;

glm::vec3 lightColour = glm::vec3(255.0f, 248.0f, 232.0f);
glm::vec3 lightPos = glm::vec3(-1.0f, 1.0f, 1.5f);
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 4.0f);

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
    depthShader = new Shader();
    depthShader->CreateFromFiles("Shaders/depthShader.vert", "Shaders/depthShader.frag");;
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
void RenderScene(glm::mat4 view, glm::mat4 projection, glm::mat4 lightView, glm::mat4 lightProjection, unsigned int texture[], GLuint depthMap){
    //draw here
    glm::vec3 modelPositions[] =
    {
        lightPos,//cubelight
        glm::vec3(0.0f, 0.0f, 0.0f),//table
        glm::vec3(0.0f, 0.845f, 0.0f),//cake
        glm::vec3(-2.2f, 2.145f, -1.8f),//yellow_balloon_1
        glm::vec3(-1.1f, 1.345f, -0.5f),//yellow_balloon_2
        glm::vec3(0.9f, 1.345f, -0.2f),//yellow_balloon_3
        glm::vec3(2.1f, 2.445f, -0.9f),//yellow_balloon_5
        glm::vec3(-2.4f, 1.945f, -1.4f),//blue_balloon_1
        glm::vec3(0.2f, 1.745f, -1.5f),//blue_balloon_3
        glm::vec3(1.1f, 2.045f, -0.6f),//blue_balloon_4
        glm::vec3(2.6f, 2.245f, -1.3f),//blue_balloon_5
        glm::vec3(-2.3f, 0.945f, -1.6f),//red_balloon_1
        glm::vec3(-1.5f, 1.045f, -0.6f),//red_balloon_2
        glm::vec3(0.4f, 1.345f, -1.3f),//red_balloon_3
        glm::vec3(1.3f, 1.245f, -0.9f),//red_balloon_4
        glm::vec3(2.3f, 1.545f, -1.1f),//red_balloon_5
        glm::vec3(-2.8f, 2.645f, -2.6f),//green_balloon_1
        glm::vec3(-1.6f, 1.545f, -0.9f),//green_balloon_2
        glm::vec3(-0.3f, 1.645f, -1.5f),//green_balloon_3
        glm::vec3(1.4f, 1.745f, -1.1f),//green_balloon_4
        glm::vec3(2.6f, 2.545f, -2.1f),//green_balloon_5
        glm::vec3(-2.2f, 0.645f, -2.8f),//pink_balloon_1
        glm::vec3(-1.8f, 1.245f, -0.7f),//pink_balloon_2
        glm::vec3(0.4f, 0.645f, -1.3f),//pink_balloon_3
        glm::vec3(1.7f, 1.345f, -1.2f),//pink_balloon_4
        glm::vec3(2.4f, 0.545f, -0.5f),//pink_balloon_5
        glm::vec3(-1.9f, 0.745f, -0.3f),//orange_balloon_2
        glm::vec3(-0.7f, 1.245f, 0.0f),//orange_balloon_3
        glm::vec3(1.5f, 1.745f, -1.5f),//orange_balloon_4
        glm::vec3(2.8f, 0.845f, -0.7f),//orange_balloon_5
        glm::vec3(0.0f, 0.0f, 0.0f),//floor
        glm::vec3(0.0f, 0.0f, 0.0f),//wall
        glm::vec3(0.5f, 2.4f, -2.0f),//banner
        glm::vec3(0.6f, 0.845f, 0.2f),//hat
        glm::vec3(-0.6f, 0.845f, 0.0f),//plate
        glm::vec3(-0.5f, 0.0f, -5.0f),//drawer
        glm::vec3(-0.5f, 1.35f, -4.5f),//TV
        glm::vec3(0.0f, 2.4f, -4.8f),//bunting
        glm::vec3(4.8f, 2.4f, 0.0),//bunting
        glm::vec3(-4.8f, 2.4f, 0.0),//bunting
        glm::vec3(0.0f, 0.0001f, 0.0),//confetti
        glm::vec3(0.0f, 0.0001f, -2.0),//confetti
        glm::vec3(2.0f, 0.0001f, -2.0),//confetti
        glm::vec3(-2.0f, 0.0001f, -2.0),//confetti
        glm::vec3(3.0f, 0.0001f, 0.0),//confetti
        glm::vec3(0.0f, 0.0001f, -3.0),//confetti
        glm::vec3(2.0f, 0.0001f, 0.0),//confetti
        glm::vec3(3.0f, 0.0001f, -3.0),//confetti
        glm::vec3(1.0f, 0.0001f, 0.0),//confetti
        glm::vec3(-1.0f, 0.0001f, 0.0),//confetti
        glm::vec3(4.2f, 0.0f, 0.0),//woodtable
        glm::vec3(4.2f, 0.9f, -1.0),//gift
    };

    float modelRotations[] =
    {
        glm::radians(0.0f),//cubelight
        glm::radians(0.5f),//table
        glm::radians(0.0f),//cake
        glm::radians(0.0f),//yellow_balloon_1
        glm::radians(0.0f),//yellow_balloon_2
        glm::radians(0.0f),//yellow_balloon_3
        glm::radians(0.0f),//yellow_balloon_5
        glm::radians(0.0f),//blue_balloon_1
        glm::radians(0.0f),//blue_balloon_3
        glm::radians(0.0f),//blue_balloon_4
        glm::radians(0.0f),//blue_balloon_5
        glm::radians(0.0f),//red_balloon_1
        glm::radians(0.0f),//red_balloon_2
        glm::radians(0.0f),//red_balloon_3
        glm::radians(0.0f),//red_balloon_4
        glm::radians(0.0f),//red_balloon_5
        glm::radians(0.0f),//green_balloon_1
        glm::radians(0.0f),//green_balloon_2
        glm::radians(0.0f),//green_balloon_3
        glm::radians(0.0f),//green_balloon_4
        glm::radians(0.0f),//green_balloon_5
        glm::radians(0.0f),//pink_balloon_1
        glm::radians(0.0f),//pink_balloon_2
        glm::radians(0.0f),//pink_balloon_3
        glm::radians(0.0f),//pink_balloon_4
        glm::radians(0.0f),//pink_balloon_5
        glm::radians(0.0f),//orange_balloon_2
        glm::radians(0.0f),//orange_balloon_3
        glm::radians(0.0f),//orange_balloon_4
        glm::radians(0.0f),//orange_balloon_5
        glm::radians(0.0f),//floor
        glm::radians(0.0f),//wall
        glm::radians(25.0f),//banner
        glm::radians(0.0f),//hat
        glm::radians(0.0f),//plate
        glm::radians(0.0f),//drawer
        glm::radians(-1.0f),//TV
        glm::radians(0.0f),//bunting
        glm::radians(90.0f),//bunting
        glm::radians(-90.0f),//bunting
        glm::radians(0.0f),//confetti
        glm::radians(0.0f),//confetti
        glm::radians(15.0f),//confetti
        glm::radians(90.0f),//confetti
        glm::radians(26.0f),//confetti
        glm::radians(30.0f),//confetti
        glm::radians(75.0f),//confetti
        glm::radians(60.0f),//confetti
        glm::radians(19.0f),//confetti
        glm::radians(64.0f),//confetti
        glm::radians(90.0f),//wood table
        glm::radians(0.0f),//gift
    };

    glm::vec3 modelScale[] =
    {
        glm::vec3(0.05f),//cubelight
        glm::vec3(1.0f),//table
        glm::vec3(0.3f),//cake
        glm::vec3(1.1f),//yellow_balloon_1
        glm::vec3(1.0f),//yellow_balloon_2
        glm::vec3(1.2f),//yellow_balloon_3
        glm::vec3(1.1f),//yellow_balloon_5
        glm::vec3(1.3f),//blue_balloon_1
        glm::vec3(1.0f),//blue_balloon_3
        glm::vec3(1.1f),//blue_balloon_4
        glm::vec3(1.0f),//blue_balloon_5
        glm::vec3(1.0f),//red_balloon_1
        glm::vec3(1.2f),//red_balloon_2
        glm::vec3(1.0f),//red_balloon_3
        glm::vec3(1.2f),//red_balloon_4
        glm::vec3(1.1f),//red_balloon_5
        glm::vec3(1.0f),//green_balloon_1
        glm::vec3(1.1f),//green_balloon_2
        glm::vec3(1.3f),//green_balloon_3
        glm::vec3(1.2f),//green_balloon_4
        glm::vec3(1.0f),//green_balloon_5
        glm::vec3(1.2f),//pink_balloon_1
        glm::vec3(1.0f),//pink_balloon_2
        glm::vec3(1.2f),//pink_balloon_3
        glm::vec3(1.1f),//pink_balloon_4
        glm::vec3(1.0f),//pink_balloon_5
        glm::vec3(1.2f),//orange_balloon_2
        glm::vec3(1.0f),//orange_balloon_3
        glm::vec3(1.1f),//orange_balloon_4
        glm::vec3(1.3f),//orange_balloon_5
        glm::vec3(1.0f),//floor
        glm::vec3(1.0f, 2.0f, 1.0f),//wall
        glm::vec3(1.0f),//banner
        glm::vec3(0.4f),//hat  
        glm::vec3(1.5f, 1.0f, 1.5f),//plate  
        glm::vec3(1.0f),//drawer  
        glm::vec3(1.0f),//TV
        glm::vec3(1.0f),//bunting
        glm::vec3(1.0f),//bunting
        glm::vec3(1.0f),//bunting
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(0.6f),//confetti
        glm::vec3(1.5f, 1.75f, 2.0f),//woodtable
        glm::vec3(1.0f),//gift
    };
    //Object
    for (int i = 0; i < models.size(); i++)
    {
        int shaderIndex = models[i].shaderIndex;
        shaderList[shaderIndex]->UseShader();
        uniformModel = shaderList[shaderIndex]->GetUniformLocation("model");
        uniformProjection = shaderList[shaderIndex]->GetUniformLocation("projection");
        uniformView = shaderList[shaderIndex]->GetUniformLocation("view");
        glUniformMatrix4fv(shaderList[shaderIndex]->GetUniformLocation("lightProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        glUniformMatrix4fv(shaderList[shaderIndex]->GetUniformLocation("lightView"), 1, GL_FALSE, glm::value_ptr(lightView));
        glm::mat4 model (1.0f);

        model = glm::translate(model, modelPositions[i]);
        model = glm::rotate(model, -modelRotations[i], glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, modelScale[i]);

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        //light
        glUniform1f(shaderList[shaderIndex]->GetUniformLocation("diffuseStrength"), (GLfloat) models[i].diffuseStrength);
        glUniform1f(shaderList[shaderIndex]->GetUniformLocation("specularStrength"), (GLfloat) models[i].specularStrength);
        glUniform1f(shaderList[shaderIndex]->GetUniformLocation("shininess"), (GLfloat) models[i].shininess);
        
        glUniform3fv(shaderList[shaderIndex]->GetUniformLocation("lightColour"), 1, (GLfloat *)&lightColour);
        glUniform3fv(shaderList[shaderIndex]->GetUniformLocation("lightPos"), 1, (GLfloat *)&lightPos);
        glUniform3fv(shaderList[shaderIndex]->GetUniformLocation("viewPos"), 1, (GLfloat *)&cameraPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(shaderList[shaderIndex]->GetUniformLocation("texture2D"), 0);
        glUniform1i(shaderList[shaderIndex]->GetUniformLocation("shadowMap"), 1);
        meshList[i]->RenderMesh();
    }
}

int main()
{
    mainWindow = Window(WIDTH, HEIGHT, 3, 3);
    mainWindow.initialise();

    CreateOBJ();
    CreateShaders();

    lightColour /= 255;

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

    //shadow
    GLuint depthMapFBO;

    glGenFramebuffers(1, &depthMapFBO);
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

    GLuint depthMap;

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
        std::cout << "Cam Pos: (" << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << ")"<< "LightPos: (" << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ")" << std::endl;
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

        //first pass: depth map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glm::mat4 lightProjection = glm::perspective(90.0f, 1.0f, 0.1f, 100.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.85f, 0.0f), up);

        depthShader->UseShader();
        uniformModel = depthShader->GetUniformLocation("model");
        uniformView = depthShader->GetUniformLocation("view");
        uniformProjection = depthShader->GetUniformLocation("projection");
        glCullFace(GL_FRONT);
        
        RenderScene(lightView, lightProjection, lightView, lightProjection, texture, depthMap);
    
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //second pass: scene
        glViewport(0, 0, mainWindow.getBufferWidth(), mainWindow.getBufferHeight());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetUniformLocation("model");
        uniformView = shaderList[0]->GetUniformLocation("view");
        uniformProjection = shaderList[0]->GetUniformLocation("projection");

        RenderScene(view, projection, lightView, lightProjection, texture, depthMap);

        glUseProgram(0);
        //end draw

        mainWindow.swapBuffers();
    }

    return 0;
}
