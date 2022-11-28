#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ImportedModel.cpp"
#include "Utils.h"
#include "SBA_socket.cpp"

using namespace std;

#define numVAOs 1
#define numVBOs 4
//VAO = Vertex Array Objects
//VBO = Vertex Buffer Objects

float cameraX , cameraY, cameraZ;
float boatLocX, boatLocY, boatLocZ;
glm::vec3 upCamera;
GLuint renderingProgram; //GLuint è una shortcat per unsigned int
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

//Variabili allocate in init così non devono essere allocate durante il rendering
GLuint mvLoc, projLoc, nLoc;
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat;
/*
 * pMat : Perspective matrix
 * vMat : View matrix
 * mMat : Model matrix
 * mvMat : vMat*mMat
 * invTrMat : Inverse Trasponse vMat
*/
glm::vec3 currentLightPos, lightPosV; //Light position in model and view space
float lightPos[3]; //Light Position in array di float
glm::vec3 initialLightLoc = glm::vec3(5.0f, 20.0f, 2.0f);// Initial Light position

//WHITE LIGHT PROPRIETY
float globalAmbient[4] = {2.0f, 2.0f, 2.0f, 1.0f};
float lightAmbient[4] = {0.0f, 0.0f, 0.0f, 1.0f};
float lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

//gold material proprieties
float * matAmb = goldAmbient();
float * matDif = goldDiffuse();
float * matSpe = goldSpecular();
float matShi = goldShininess();
void installLights(glm::mat4);

ImportedModel monkey("Sail.obj");

void setupVertices(void){
    std::vector<glm::vec3> vert = monkey.getVertices();
    std::vector<glm::vec2> tex = monkey.getTextCoords();
    std::vector<glm::vec3> norm = monkey.getNormalVecs();
    int numObjVertices = monkey.getNumVertices();

    std::vector<float> pvalues; //vertex positions
    std::vector<float> tvalues; //texture coordinates
    std::vector<float> nvalues; //normal vectors

    for (int i=0; i<numObjVertices; i++){
        pvalues.push_back((vert[i]).x);
        pvalues.push_back((vert[i]).y);
        pvalues.push_back((vert[i]).z);
        tvalues.push_back((tex[i]).s);
        tvalues.push_back((tex[i]).t);
        nvalues.push_back((norm[i]).x);
        nvalues.push_back((norm[i]).y);
        nvalues.push_back((norm[i]).z);
    }
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    //VBO for vertex location
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);

    //VBO for texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);

    //VBO for normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);

}

void init (GLFWwindow* window){
    renderingProgram = createShaderProgram((char *)"vertShader.glsl",(char *) "fragShader.glsl");
    cameraX = 10.0f; cameraY = 7.0f; cameraZ = 10.0f;
    boatLocX = 0.0f; boatLocY = -2.0f; boatLocZ = 0.0f;
    setupVertices();

    //Costruisco la pMat
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); //1.0472 radians = 60 degrees
}

void display (GLFWwindow* window, double currentTime){
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT); //Clear the background to black
    glUseProgram(renderingProgram);
    //glEnable(GL_CULL_FACE);

    //Getto le uniform
    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
    nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

    //Aggiorno la posizione della camera
    //cameraZ = (28+20*cos((float)currentTime))*sin((float)currentTime*0.6);
    //cameraX = (28+20*cos((float)currentTime))*cos((float)currentTime*0.6);
    //cameraY = (7+5*cos((float)currentTime));

    boatLocX = (float)(10*cos((float)currentTime*1));

    //Costruisco la mvMat
    vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(boatLocX, boatLocY, boatLocZ));
    glm::mat4 rotation_phi = glm::rotate(glm::mat4(1.0f), (float)(0.1*cos((float)currentTime*5)), glm::vec3(1.0f,0.0f,0.0f));
    glm::vec4 new_y_axis = rotation_phi*glm::vec4(0.0f,1.0f,0.0f,1.0f);
    //glm::mat4 rotation_xi = glm::rotate(glm::mat4(1.0f), (float)(cos((float)currentTime*0.6)), glm::vec3(new_y_axis[0], new_y_axis[1], new_y_axis[2]));
    glm::mat4 rotation_xi = glm::rotate(glm::mat4(1.0f), (float)(cos((float)currentTime*0.6)), glm::vec3(0.0f, 1.0f, 0.0f));
    //std::cout << "[" << new_y_axis[0] << "," << new_y_axis[1] << "," << new_y_axis[2] << "," << new_y_axis[3] << "]" << std::endl;
    mMat = mMat * rotation_xi;
    mMat = mMat * rotation_phi;
    //mMat *= glm::rotate(glm::mat4(1.0f), 2.0f*(float)(cos((float)currentTime*0.6)), glm::vec3(1.0f,0.0f,0.0f));
    //mMat *= glm::rotate(glm::mat4(1.0f), 2.0f*(float)(cos((float)currentTime*0.6)), glm::vec3(0.0f,1.0f,0.0f));

    //Set up lights
    currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);
    installLights(vMat);

    mvMat = vMat * mMat;

    //Build matrix for trasforming vectors
    invTrMat = glm::transpose(glm::inverse(mvMat));

    //Spedisco matrici allo shader
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));

    //Associazione VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, monkey.getNumVertices());
}

void installLights(glm::mat4 vMat){
    //Convert light's position to view space and prepare shader data sending
    lightPosV = glm::vec3(vMat*glm::vec4(currentLightPos, 1.0));
    lightPos[0] = lightPosV.x;
    lightPos[1] = lightPosV.y;
    lightPos[2] = lightPosV.z;

    //get locations
    globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
    ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
    diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
    specLoc = glGetUniformLocation(renderingProgram, "light.specular");
    posLoc = glGetUniformLocation(renderingProgram, "light.position");
    mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
    mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
    mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
    mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");

    //set uniforms
    glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
    glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
    glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
    glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
    glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
    glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
    glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
    glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpe);
    glProgramUniform1f(renderingProgram, mShiLoc, matShi);

}

void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight){
    aspect = (float)newWidth/(float)newHeight;
    glViewport(0, 0, newWidth, newHeight);
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); //1.0472 radians = 60 degrees
}

int main(void){
    MySocketClient MSC = MySocketClient();
    if (!glfwInit()) {exit(EXIT_FAILURE);}
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(600, 600, "Catamarano", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK){exit(EXIT_FAILURE);}
    glfwSwapInterval(1);

    glfwSetWindowSizeCallback(window, window_reshape_callback);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        MSC.reading();
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

