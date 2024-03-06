#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"
#include <iostream>

// window
gps::Window myWindow;
int glWindowWidth = 900;
int glWindowHeight = 700;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 3.0f, 10.0f),   // Camera Position
    glm::vec3(0.0f, 0.0f, -10.0f),  // Camera Target
    glm::vec3(0.0f, 1.0f, 0.0f));   // Up Direction
GLfloat cameraSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D teapot;
GLfloat angle;
gps::Model3D character;
glm::mat4 characterModel;

gps::Model3D streetlight;
glm::mat4 streetlightModel;

gps::Model3D boat;
glm::mat4 boatModel;
// shaders
gps::Shader myBasicShader;

//rotation
glm::mat4 characterTranslation = glm::mat4(1.0f);
glm::mat4 characterRotation = glm::mat4(1.0f);
glm::mat4 characterScale = glm::mat4(1.0f);


GLenum polygonMode = GL_FILL;  // Initial mode is solid

//skybox
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;
std::vector<const GLchar*> faces;
//lumina punctiforma
 bool isPunctiformLightActive = false;
 bool isZKeyProcessed = false;
 //GLint lightPos;

 //ceata
 bool isFogActive = false;
 bool isXKeyProcessed = false;
 //prezentare
 bool isPresentationActive = false;
 float presentationTime = 0.0f;
 //umbre
 
void togglePresentationMode() {
    isPresentationActive = !isPresentationActive;
    presentationTime = 0.0f; // Reset time when toggling
}
GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);

    glViewport(0, 0, width, height);

    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);

    myBasicShader.useShaderProgram();
    projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    //lightPosLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightPos");

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
   
}
bool firstMouse = true;
float lastX = glWindowWidth / 2;
float lastY = glWindowHeight / 2;
float yaw = -90.0f, pitch = 0.0f;
const float sensitivity = 0.3f;
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;


    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 88.0f) pitch = 88.0f;
    if (pitch < -88.0f) pitch = -88.0f;

    myCamera.rotate(pitch, yaw);
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void processMovement() {
    float cameraSpeed = 0.05f;

    
    if (pressedKeys[GLFW_KEY_LEFT_SHIFT]) {
        myCamera.move(gps::MOVE_UP, cameraSpeed);
    }

    
    if (pressedKeys[GLFW_KEY_TAB]) {
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
    }

    
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    }
    if (pressedKeys[GLFW_KEY_UP]) {
        characterModel = glm::translate(characterModel, glm::vec3(0.0f, 0.0f, 0.1f));
    }

    if (pressedKeys[GLFW_KEY_DOWN]) {
        characterModel = glm::translate(characterModel, glm::vec3(0.0f, 0.0f, -0.1f));
    }

    if (pressedKeys[GLFW_KEY_LEFT]) {
        characterModel = glm::translate(characterModel, glm::vec3(-0.1f, 0.0f, 0.0f));
    }

    if (pressedKeys[GLFW_KEY_RIGHT]) {
        characterModel = glm::translate(characterModel, glm::vec3(0.1f, 0.0f, 0.0f));
    }
    if (pressedKeys[GLFW_KEY_N]) {
        characterModel = glm::translate(characterModel, glm::vec3(0.0f, 0.1f, 0.0f));
    }
    if (pressedKeys[GLFW_KEY_M]) {
        characterModel = glm::translate(characterModel, glm::vec3(0.0f, -0.1f, 0.0f));
    }
    if (pressedKeys[GLFW_KEY_B]) {
        angle -= 1.0f;
        characterModel = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    }
    // Scaling with 'K' and 'L' keys
    const float scaleSpeed = 0.01f;
    if (pressedKeys[GLFW_KEY_K]) {
        characterModel = glm::scale(characterModel, glm::vec3(1.0f - scaleSpeed));
    }

    if (pressedKeys[GLFW_KEY_L]) {
        characterModel = glm::scale(characterModel, glm::vec3(1.0f + scaleSpeed));
    }
    if (pressedKeys[GLFW_KEY_1]) {
        polygonMode = GL_FILL;  // Solid mode
    }

    if (pressedKeys[GLFW_KEY_2]) {
        polygonMode = GL_LINE;  // Wireframe mode
    }

    if (pressedKeys[GLFW_KEY_3]) {
        polygonMode = GL_POINT;  // Point mode
    }

    if (pressedKeys[GLFW_KEY_4]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  // Smooth shading
        glEnable(GL_POLYGON_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else {
        glDisable(GL_POLYGON_SMOOTH);
        glDisable(GL_BLEND);
    }
    // Toggle punctiform light with 'Z' key
    if (pressedKeys[GLFW_KEY_Z] && !isZKeyProcessed) {
        isPunctiformLightActive = !isPunctiformLightActive; 
        isZKeyProcessed = true; 
    }
    if (!pressedKeys[GLFW_KEY_Z]) {
        isZKeyProcessed = false; 
    }

    if (pressedKeys[GLFW_KEY_X] && !isXKeyProcessed) {
        isFogActive = !isFogActive;
        isXKeyProcessed = true;
    }

    if (!pressedKeys[ GLFW_KEY_X] ) {
        isXKeyProcessed = false;
    }
    if (pressedKeys[GLFW_KEY_SPACE]) {
        togglePresentationMode();
    }
    
    
    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * characterModel));
}



void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    //teapot.LoadModel("models/teapot/teapot20segUT.obj");
    teapot.LoadModel("models/peaceful/scene.obj");
    character.LoadModel("models/catModel/catfinalmodel4.obj");
    streetlight.LoadModel("models/Felinar/lamp_sp_01.obj");
    boat.LoadModel("models/peaceful/boat.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
}

void initUniforms() {
	myBasicShader.useShaderProgram();
    characterModel = glm::mat4(1.0f);
    streetlightModel = glm::mat4(1.0f);
    boatModel = glm::mat4(1.0f);
    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void renderTeapot(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send teapot normal matrix data to shader
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw teapot
    teapot.Draw(shader);
}
void renderCharacter(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send teapot model matrix data to shader
   // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(characterModel));
    //send teapot normal matrix data to shader
   //glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(glm::mat3(glm::inverseTranspose(view * characterModel))));
    // draw teapot
    character.Draw(shader);

 
}
void renderStreetLight(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(streetlightModel));
   
   glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
 
    streetlight.Draw(shader);


}
void renderBoat(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(boatModel));
    
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
   
    boat.Draw(shader);


}
glm::vec3 getCharacterModelPosition() {
   
    return glm::vec3(characterModel[3]);
}
glm::vec3 getStreetlightPosition() {
    return glm::vec3(streetlightModel[3]);
}
void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLint lightPos=glGetUniformLocation(myBasicShader.shaderProgram, "lightPos");
	//render the scene
     // Set the polygon mode
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode); 
    //point light
    glm::vec3 streetlightPos = getCharacterModelPosition();
    GLint lightActiveLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isLightActive");
    glUniform1i(lightActiveLoc, isPunctiformLightActive ? 1 : 0);
    glUniform3fv(lightPos, 1, glm::value_ptr(streetlightPos));
    //fog
    GLint fogActiveLoc = glGetUniformLocation(myBasicShader.shaderProgram, "isFogActive");
    glUniform1i(fogActiveLoc, isFogActive ? 1 : 0);

	
	renderTeapot(myBasicShader);
    renderCharacter(myBasicShader);
    renderStreetLight(myBasicShader);
    renderBoat(myBasicShader);
    mySkyBox.Draw(skyboxShader, view, projection);

}
void presentation() {
    if (!isPresentationActive) {
        return;
    }

    presentationTime += 0.005f;

    // Phase 1: Camera follows the boat
    if (presentationTime < 5.0f) { 
        float radius = 3.0f; 
        float offsetX = 2.0f; 
        float offsetZ = 0.0f; 

        glm::vec3 boatPosition = glm::vec3(radius * sin(presentationTime) + offsetX, 0.0f, radius * cos(presentationTime) + offsetZ);

        boatModel = glm::translate(glm::mat4(1.0f), boatPosition);

        glm::vec3 cameraOffset = glm::vec3(0.0f, 3.0f, 10.0f);
        glm::vec3 cameraPos = boatPosition + cameraOffset;
        glm::vec3 cameraTarget = boatPosition;
        myCamera.setPosition(cameraPos);
        myCamera.setTarget(cameraTarget);
    }
    
    else {
        // Rotate the camera to view the entire scene
        float overviewTime = presentationTime - 5.0f; 
        glm::vec3 overviewPosition = glm::vec3(13.0f * sin(overviewTime), 7.0f, 8.0f * cos(overviewTime));
        glm::vec3 overviewTarget = glm::vec3(1.0f, 3.0f, 4.0f);

        myCamera.setPosition(overviewPosition);
        myCamera.setTarget(overviewTarget);
    }
}



void initSkyBox() {
    faces.push_back("skybox/greenhaze_rt.tga");  // Right
    faces.push_back("skybox/greenhaze_lf.tga");  // Left
    faces.push_back("skybox/greenhaze_up.tga");  // Up
    faces.push_back("skybox/greenhaze_dn.tga");  // Down
    faces.push_back("skybox/greenhaze_bk.tga");  // Back
    faces.push_back("skybox/greenhaze_ft.tga");  // Front

    mySkyBox.Load(faces);
}




void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    initSkyBox();
    setWindowCallbacks();
    

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        presentation();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
