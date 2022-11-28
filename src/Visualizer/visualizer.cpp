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

#define numVAOs 2
#define numVBOs 4
//VAO = Vertex Array Objects
//VBO = Vertex Buffer Objects

#define PASSOVISUALE 0.1f
#define PASSOCAMERA 1.0f

float cameraX , cameraY, cameraZ;
float lookingDirX, lookingDirY, lookingDirZ;
float boatLocX, boatLocY, boatLocZ;

bool cameraAttacedToBoat = false;
float backmousex=0, backmousey=0;
bool mouseblocked = false;

glm::vec3 upCamera;
GLuint renderingProgram; //GLuint è una shortcat per unsigned int
GLuint vao[numVAOs];
GLuint vbo_sail[numVBOs];
GLuint vbo_boat[numVBOs];
GLuint vbo_insland[numVBOs];

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
glm::vec3 initialLightLoc = glm::vec3(0.0f, 100.0f, 0.0f);// Initial Light position

//WHITE LIGHT PROPRIETY
float globalAmbient[4] = {2.0f, 2.0f, 2.0f, 1.0f};
float lightAmbient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
float lightDiffuse[4] = {1.0f, 1.0f, 1.0f, 1.0f};
float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};

//gold material proprieties
float * matAmb = goldAmbient();
float * matDif = goldDiffuse();
float * matSpe = goldSpecular();
float matShi = goldShininess();
void installLights(glm::mat4);

ImportedModel sail("Sail.obj");
ImportedModel boat("Sail_Boat_Without_Sails.obj");
ImportedModel insland("Colline.obj");

void setupVertices(void){
    std::vector<glm::vec3> vert_boat = boat.getVertices();
    std::vector<glm::vec2> tex_boat = boat.getTextCoords();
    std::vector<glm::vec3> norm_boat = boat.getNormalVecs();
    int numObjVertices = boat.getNumVertices();

    std::vector<float> pvalues_boat; //vertex positions
    std::vector<float> tvalues_boat; //texture coordinates
    std::vector<float> nvalues_boat; //normal vectors

    for (int i=0; i<numObjVertices; i++){
        pvalues_boat.push_back((vert_boat[i]).x);
        pvalues_boat.push_back((vert_boat[i]).y);
        pvalues_boat.push_back((vert_boat[i]).z);
        tvalues_boat.push_back((tex_boat[i]).s);
        tvalues_boat.push_back((tex_boat[i]).t);
        nvalues_boat.push_back((norm_boat[i]).x);
        nvalues_boat.push_back((norm_boat[i]).y);
        nvalues_boat.push_back((norm_boat[i]).z);
    }
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo_boat);

    //VBO for vertex location
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boat[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues_boat.size()*4, &pvalues_boat[0], GL_STATIC_DRAW);

    //VBO for texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boat[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues_boat.size()*4, &tvalues_boat[0], GL_STATIC_DRAW);

    //VBO for normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boat[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues_boat.size()*4, &nvalues_boat[0], GL_STATIC_DRAW);
    
    std::vector<glm::vec3> vert_sail = sail.getVertices();
    std::vector<glm::vec2> tex_sail = sail.getTextCoords();
    std::vector<glm::vec3> norm_sail = sail.getNormalVecs();
    numObjVertices = sail.getNumVertices();

    std::vector<float> pvalues_sail; //vertex positions
    std::vector<float> tvalues_sail; //texture coordinates
    std::vector<float> nvalues_sail; //normal vectors

    for (int i=0; i<numObjVertices; i++){
        pvalues_sail.push_back((vert_sail[i]).x);
        pvalues_sail.push_back((vert_sail[i]).y);
        pvalues_sail.push_back((vert_sail[i]).z);
        tvalues_sail.push_back((tex_sail[i]).s);
        tvalues_sail.push_back((tex_sail[i]).t);
        nvalues_sail.push_back((norm_sail[i]).x);
        nvalues_sail.push_back((norm_sail[i]).y);
        nvalues_sail.push_back((norm_sail[i]).z);
    }
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo_sail);

    //VBO for vertex location
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sail[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues_sail.size()*4, &pvalues_sail[0], GL_STATIC_DRAW);

    //VBO for texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sail[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues_sail.size()*4, &tvalues_sail[0], GL_STATIC_DRAW);

    //VBO for normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sail[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues_sail.size()*4, &nvalues_sail[0], GL_STATIC_DRAW);
    
    std::vector<glm::vec3> vert_island = insland.getVertices();
    std::vector<glm::vec2> tex_island = insland.getTextCoords();
    std::vector<glm::vec3> norm_island = insland.getNormalVecs();
    numObjVertices = insland.getNumVertices();

    std::vector<float> pvalues_insland; //vertex positions
    std::vector<float> tvalues_insland; //texture coordinates
    std::vector<float> nvalues_insland; //normal vectors

    for (int i=0; i<numObjVertices; i++){
        pvalues_insland.push_back((vert_island[i]).x);
        pvalues_insland.push_back((vert_island[i]).y);
        pvalues_insland.push_back((vert_island[i]).z);
        tvalues_insland.push_back((tex_island[i]).s);
        tvalues_insland.push_back((tex_island[i]).t);
        nvalues_insland.push_back((norm_island[i]).x);
        nvalues_insland.push_back((norm_island[i]).y);
        nvalues_insland.push_back((norm_island[i]).z);
    }
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo_insland);

    //VBO for vertex location
    glBindBuffer(GL_ARRAY_BUFFER, vbo_insland[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues_insland.size()*4, &pvalues_insland[0], GL_STATIC_DRAW);

    //VBO for texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, vbo_insland[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues_insland.size()*4, &tvalues_insland[0], GL_STATIC_DRAW);

    //VBO for normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, vbo_insland[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues_insland.size()*4, &nvalues_insland[0], GL_STATIC_DRAW);
}

void init (GLFWwindow* window){
    renderingProgram = createShaderProgram((char *)"vertShader.glsl",(char *) "fragShader.glsl");
    cameraX = 30.0f; cameraY = 14.0f; cameraZ = 30.0f;
    boatLocX = 0.0f; boatLocY = 2.0f; boatLocZ = 0.0f;
    setupVertices();

    //Costruisco la pMat
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); //1.0472 radians = 60 degrees
    lookingDirX = -1; lookingDirY = 0; lookingDirZ = 0;
}

void display (GLFWwindow* window, double currentTime){
    double phi = cos((float)currentTime*0.1);
    double xi = cos((float)currentTime*0.6);
    double lambda = cos((float)currentTime*3);
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
    vMat = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(cameraX+lookingDirX, cameraY+lookingDirY, cameraZ+lookingDirZ), glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(boatLocX, boatLocY, boatLocZ));
    glm::mat4 rotation_phi = glm::rotate(glm::mat4(1.0f), (float)(phi), glm::vec3(1.0f,0.0f,0.0f));
    glm::mat4 rotation_xi = glm::rotate(glm::mat4(1.0f), (float)(xi), glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = mMat * rotation_xi;
    mMat = mMat * rotation_phi;


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
    glBindBuffer(GL_ARRAY_BUFFER, vbo_boat[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_boat[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, boat.getNumVertices());
    
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(boatLocX, boatLocY, boatLocZ));
    glm::vec4 new_y = rotation_phi*glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    glm::mat4 rotation_lambda = glm::rotate(glm::mat4(1.0f), (float)(lambda), glm::vec3(0.0f, 1.0f, 0.0f));
    mMat = mMat * rotation_xi;
    mMat = mMat * rotation_phi;
    mMat = mMat * rotation_lambda;
    mvMat = vMat * mMat;
    
    //Spedisco matrici allo shader
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    
    //Associazione VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo_sail[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_sail[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, sail.getNumVertices());
    
    mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
    mMat *= glm::scale(glm::mat4(1.0f), glm::vec3(10.f, 10.f, 10.f));
    mvMat = vMat * mMat;
    
    //Spedisco matrici allo shader
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
    
    //Associazione VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo_insland[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_insland[2]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    // glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDrawArrays(GL_TRIANGLES, 0, insland.getNumVertices());
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    //cout<<"KEY:"<<key<<" - "<<glfwGetKeyScancode(key)<<endl;
    /*FrecciaSu : 265
     *FrecciaGiu : 264
     *FrecciaSx : 263
     *FrecciaDx : 262
    */
    if (glfwGetKey(window, 265) == GLFW_PRESS){//freccia su
      cameraAttacedToBoat = true;
    }
    if (glfwGetKey(window, 264) == GLFW_PRESS){//freccia giu
      cameraAttacedToBoat = false;
    }
    if (glfwGetKey(window, 263) == GLFW_PRESS){//freccia sinistra
    }
    if (glfwGetKey(window, 262) == GLFW_PRESS){//freccia destra
    }
    if (glfwGetKey(window, 87) == GLFW_PRESS){//W
      if (!cameraAttacedToBoat){
        cameraX += lookingDirX*PASSOCAMERA;
        cameraZ += lookingDirZ*PASSOCAMERA;
      }
    }
    if (glfwGetKey(window, 83) == GLFW_PRESS){//S
      if (!cameraAttacedToBoat){
        cameraX -= lookingDirX*PASSOCAMERA;
        cameraZ -= lookingDirZ*PASSOCAMERA;
      }
    }
    if (glfwGetKey(window, 256) == GLFW_PRESS){// esc
        if (mouseblocked){
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
          mouseblocked = false;
        }else{
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
          mouseblocked = true;
        }
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
  if (mouseblocked && !cameraAttacedToBoat){
    glm::vec3 newlookingDir(lookingDirX, lookingDirY, lookingDirZ);
    float newlookingDirX = lookingDirX;
    float newlookingDirZ = lookingDirZ;
    float newlookingDirY = lookingDirY;
    float thetax = PASSOVISUALE*abs(backmousex-xpos)/50;
    float thetay = PASSOVISUALE*abs(backmousey-ypos)/200;
    if (xpos>backmousex){
      newlookingDir = rotate(0, 1, 0, glm::vec3(lookingDirX, lookingDirY, lookingDirZ), thetax);
    }
    if (xpos<backmousex){
      newlookingDir = rotate(0, 1, 0, glm::vec3(lookingDirX, lookingDirY, lookingDirZ), -thetax);
    }
    if (ypos>backmousey && atan(lookingDirY/(sqrt(lookingDirX*lookingDirX+lookingDirZ*lookingDirZ)))>-0.785398f){
      newlookingDir = rotate(-(lookingDirZ)/sqrt(lookingDirX*lookingDirX+lookingDirZ*lookingDirZ), 0, (lookingDirX)/sqrt(lookingDirX*lookingDirX+lookingDirZ*lookingDirZ), glm::vec3(lookingDirX, lookingDirY, lookingDirZ), thetay);
    }
    if (ypos<backmousey && atan(lookingDirY/(sqrt(lookingDirX*lookingDirX+lookingDirZ*lookingDirZ)))<0.785398f){
      newlookingDir = rotate(-(lookingDirZ)/sqrt(lookingDirX*lookingDirX+lookingDirZ*lookingDirZ), 0, (lookingDirX)/sqrt(lookingDirX*lookingDirX+lookingDirZ*lookingDirZ), glm::vec3(lookingDirX, lookingDirY, lookingDirZ), -thetay);
    }
    lookingDirX = newlookingDir.x;
    lookingDirY = newlookingDir.y;
    lookingDirZ = newlookingDir.z;
    backmousex = xpos;
    backmousey = ypos;
    //cout << xpos << ";" << ypos << endl;
  }
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
    
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    mouseblocked=true;
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

