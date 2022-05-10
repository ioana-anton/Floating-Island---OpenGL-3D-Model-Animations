//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright � 2016 CGIS. All rights reserved.
//

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

#include <iostream>

#define M_PI       3.14159265358979323846f

int glWindowWidth = 1200;
int glWindowHeight = 800;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

const unsigned int SHADOW_WIDTH = 8192;
const unsigned int SHADOW_HEIGHT = 8192;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

gps::Camera myCamera(
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));
float cameraSpeed = 0.3f;
float mouseSpeed = 0.002f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D nanosuit;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D island;
gps::Model3D lantern_light;
gps::Model3D star;
gps::Model3D waterfall;
gps::Model3D pink_petal;
gps::Model3D purple_petal;
gps::Model3D small_sphere;
gps::Model3D torus_sphere_1;
gps::Model3D torus_sphere_2;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthShader;
gps::Shader waterShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

float constant = 1.0f;
GLuint constantLoc;
float linear = 0.7f;
GLuint linearLoc;
float quadratic = 1.8f;
GLuint quadraticLoc;

glm::vec3 initialPosLanternLight = glm::vec3(2.9531f, 0.64537f, -2.9192f);
glm::vec3 posLanternLight = glm::vec3(2.9531f, 0.64537f, -2.9192f);
GLuint posLanternLightLoc;
float lanternStep = 0.0f;
int lanternDirection = 0;

glm::vec3 posStarLight = glm::vec3(-4.5948, 12.601, 1.7764);
GLuint posStarLightLoc;

float lightIntensity = 0.5f;
GLuint lightIntensityLoc;

bool showDepthMap;

float fogDensity = 0.0f;
GLuint fogDensityLoc;

float starRotation = 0.0f;

float petalMovement = 0.02f;
glm::vec3 initialPetalPosition = glm::vec3(-1.84768f, 0.977086f, 2.25244f);
int numberOfPetals = 6;
glm::vec3 petals[100];
int initialPetalsY[100];

glm::vec3 initialSmallSpherePosition = glm::vec3(-4.3581f, 7.4468f, -1.7796f);
float sphereStep = 0.0f;
int sphereDirection = 0;

glm::vec3 initialTorus1Position = glm::vec3(-4.60737f, 12.6984f, -1.86542f);
glm::vec3 initialTorus2Position = glm::vec3(-4.60737f, 12.6984f, -1.86542f);
float torusStep = 0.0f;
int torusDirection = 0;

struct cameraFromFile {
	glm::vec3 position, target, up;
	float pitch, yaw;
};

std::vector<cameraFromFile> presentationCamera;

bool presenting = false;
long presentationId = 0;
long presentation_cnt = 0;
const long presentation_delay = 1;

bool recording = false;

std::ofstream g("../cameraout");
void saveInFileCamera() {
	glm::vec3 pos = myCamera.getCameraPosition();
	glm::vec3 target = myCamera.getCameraTarget();
	glm::vec3 up = myCamera.getCameraTarget();
	float pitch = myCamera.getCameraPitch();
	float yaw = myCamera.getCameraYaw();

	g << pos.x << " " << pos.y << " " << pos.z << " "
		<< target.x << " " << target.y << " " << target.z
		<< " " << up.x << " " << up.y << " " << up.z << " "
		<< pitch << " " << yaw << " ";
}

void loadPresentation() {
	std::ifstream f("./camera");
	float px, py, pz;
	float tx, ty, tz;
	float ux, uy, uz;
	float pitch, yaw;
	while (f >> px >> py >> pz >> tx >> ty >> tz >> ux >> uy >> uz >> pitch >> yaw) {
		presentationCamera.push_back({ glm::vec3(px, py, pz),
									  glm::vec3(tx, ty, tz),
									  glm::vec3(ux, uy, uz),
										pitch, yaw
			});
	}
	f.close();
	presenting = false;
	std::cout << "Am incarcat:" << presentationCamera.size() << "\n";
}

glm::vec3 initial_pos;
glm::vec3 initial_target;
glm::vec3 initial_up;
float initial_pitch;
float initial_yaw;

GLenum glCheckError_(const char* file, int line) {
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO	
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (presenting) return;
	myCamera.rotate(mouseSpeed * ((float)glWindowHeight / 2.0f - ypos), mouseSpeed * ((float)glWindowWidth / 2.0f - xpos));
	glfwSetCursorPos(window, glWindowWidth / 2, glWindowHeight / 2);
	if (recording) saveInFileCamera();
}

void processMovement()
{

	//Vizualizarea scenei

	if (pressedKeys[GLFW_KEY_1]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //solid
	}

	if (pressedKeys[GLFW_KEY_2]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
	}

	if (pressedKeys[GLFW_KEY_3]) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}

	if (pressedKeys[GLFW_KEY_Q]) {
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E]) {
		angleY += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J]) {
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L]) {
		lightAngle += 1.0f;
	}

	if (!presenting) {
		if (pressedKeys[GLFW_KEY_W]) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
			if (recording) saveInFileCamera();
		}

		if (pressedKeys[GLFW_KEY_S]) {
			myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
			if (recording) saveInFileCamera();
		}

		if (pressedKeys[GLFW_KEY_A]) {
			myCamera.move(gps::MOVE_LEFT, cameraSpeed);
			if (recording) saveInFileCamera();
		}

		if (pressedKeys[GLFW_KEY_D]) {
			myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
			if (recording) saveInFileCamera();
		}

		if (pressedKeys[GLFW_KEY_SPACE]) {
			myCamera.move(gps::MOVE_UP, cameraSpeed);
			if (recording) saveInFileCamera();
		}
	}

	if (pressedKeys[GLFW_KEY_LEFT_BRACKET]) {
		if (fogDensity >= 0.005f)
			fogDensity -= 0.005f;
	}

	if (pressedKeys[GLFW_KEY_RIGHT_BRACKET]) {
		if (fogDensity <= 0.7f)
			fogDensity += 0.005f;
	}

	if (pressedKeys[GLFW_KEY_MINUS]) {
		if (lightIntensity <= 1.0f)
			lightIntensity += 0.05f;
	}

	if (pressedKeys[GLFW_KEY_0]) {
		if (lightIntensity > 0.05f)
			lightIntensity -= 0.05f;
	}

	if (pressedKeys[GLFW_KEY_K]) {
		recording = true;
	}

	if (pressedKeys[GLFW_KEY_I]) {
		recording = false;
	}

	if (pressedKeys[GLFW_KEY_C]) {
		if (presenting) return;

		initial_pos = myCamera.getCameraPosition();
		initial_target = myCamera.getCameraTarget();
		initial_up = myCamera.getCameraUpDirection();
		initial_pitch = myCamera.getCameraPitch();
		initial_yaw = myCamera.getCameraYaw();
		presentationId = 0;
		presentation_cnt = 0;
		presenting = true;
	}

	if (pressedKeys[GLFW_KEY_V]) {
		if (!presenting) return;

		presenting = false;
		myCamera.setCameraParams(initial_pos, initial_target, initial_up, initial_pitch, initial_yaw, true);
	}
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	glfwSetCursorPos(glWindow, glWindowWidth / 2, glWindowHeight / 2);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3, 0.3, 0.3, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	//glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise

	glEnable(GL_BLEND);

	glEnable(GL_FRAMEBUFFER_SRGB);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void initObjects() {
	nanosuit.LoadModel("objects/nanosuit/nanosuit.obj");
	ground.LoadModel("objects/ground/ground.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");

	island.LoadModel("objects/floating_island/floating_island.obj");
	lantern_light.LoadModel("objects/lantern_light/lantern_light.obj");
	star.LoadModel("objects/star/star.obj");
	waterfall.LoadModel("objects/waterfall/waterfall.obj");
	pink_petal.LoadModel("objects/pink_petal/pink_petal.obj");
	purple_petal.LoadModel("objects/purple_petal/purple_petal.obj");

	small_sphere.LoadModel("objects/small_sphere/small_sphere.obj");
	torus_sphere_1.LoadModel("objects/torus_sphere_1/torus_sphere_1.obj");
	torus_sphere_2.LoadModel("objects/torus_sphere_2/torus_sphere_2.obj");
}


void initPetals() {

	float step = 0.2f;

	for (int i = 0; i < numberOfPetals; i++) {
		//petals[i] = glm::vec3(initialPetalPosition.x + step * (i + 1), initialPetalPosition.y, initialPetalPosition.z + step * (i + 1));
		petals[i].x = initialPetalPosition.x + step * (i + 1) + (rand() % 100 + 1) / 100;
		petals[i].y = initialPetalPosition.y + step * (i + 1) + (rand() % 100 + 1) / 100;
		petals[i].z = initialPetalPosition.z + step * (i + 1) + (rand() % 100 + 1) / 100;
		initialPetalPosition[i] = petals[i].y;
		printf("Petala %d : x: %f y: %f z: %f\n", i, petals[i].x, petals[i].y, petals[i].z);
	}

}

void initSkyBox() {
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/right.tga");
	faces.push_back("skybox/left.tga");
	faces.push_back("skybox/top.tga");
	faces.push_back("skybox/bottom.tga");
	faces.push_back("skybox/back.tga");
	faces.push_back("skybox/front.tga");

	mySkyBox.Load(faces);
	skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	skyboxShader.useShaderProgram();
	view = myCamera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
		glm::value_ptr(view));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
		glm::value_ptr(projection));
}

void initShaders() {
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();
	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();
	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();

	depthShader.loadShader("shaders/depthShader.vert", "shaders/depthShader.frag");
	depthShader.useShaderProgram();

	waterShader.loadShader("shaders/waterShader.vert", "shaders/waterShader.frag");
	waterShader.useShaderProgram();
}

void initUniforms() {
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 4.0f, 5.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	//set light color
	lightColor = glm::vec3(lightIntensity, lightIntensity, lightIntensity); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	//Fog 
	fogDensityLoc = glGetUniformLocation(myCustomShader.shaderProgram, "fogDensity");
	glUniform1f(fogDensityLoc, fogDensity);

	//pentru lumina punctiforma
	constantLoc = glGetUniformLocation(myCustomShader.shaderProgram, "constant");
	glUniform1f(constantLoc, constant);

	linearLoc = glGetUniformLocation(myCustomShader.shaderProgram, "linear");
	glUniform1f(linearLoc, linear);

	quadraticLoc = glGetUniformLocation(myCustomShader.shaderProgram, "quadratic");
	glUniform1f(quadraticLoc, quadratic);

	posLanternLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "uPosLanternLight");
	glUniform3fv(posLanternLightLoc, 1, glm::value_ptr(posLanternLight));

	//starLight
	posStarLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "uPosStarLight");
	glUniform3fv(posStarLightLoc, 1, glm::value_ptr(posStarLight));

	waterShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix3fv(glGetUniformLocation(waterShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "fogDensity"), fogDensity);

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void initFBO() {
	//TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	//generate FBO ID
	glGenFramebuffers(1, &shadowMapFBO);

	//create depth texture for FBO
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//attach texture to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//transformari in spatiul luminii
	glm::mat4 lightView = glm::lookAt(lightDir, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	const GLfloat near_plane = 0.1f, far_plane = 5.0f;
	glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);

	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

}

glm::mat4 computeLightSpaceTrMatrix() {
	//TODO - Return the light-space transformation matrix
	float k = 5.0f;
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightView = glm::lookAt(glm::mat3(lightRotation) * lightDir, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = .01f, far_plane = 20.0f;

	glm::mat4 lightProjection = glm::ortho(-k, k, -k, k, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
	return lightSpaceTrMatrix;
	return glm::mat4(1.0f);
}

void updateLanternLightUniform() {
	posLanternLightLoc = glGetUniformLocation(myCustomShader.shaderProgram, "uPosLanternLight");
	glUniform3fv(posLanternLightLoc, 1, glm::value_ptr(posLanternLight));
}

void drawObjects(gps::Shader shader, bool depthPass) {

	shader.useShaderProgram();

	if (!depthPass) {
		glUniform1f(fogDensityLoc, fogDensity);
		glUniform1f(lightIntensityLoc, lightIntensity);
		lightColor = glm::vec3(lightIntensity, lightIntensity, lightIntensity);
		glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
	}

	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	// do not send the normal matrix if we are rendering in the depth map
	if (!depthPass) {
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	//animatie stea
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.5948, 12.601, 1.7764));
	model = glm::rotate(model, M_PI * starRotation, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, -glm::vec3(-4.5948, 12.601, 1.7764));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	star.Draw(shader);
	starRotation += 0.01f;
	if (starRotation >= 2.0f) starRotation = 0.0f;

	//animatie sfera mica
	model = glm::mat4(1.0f);
	//model = glm::translate(glm::mat4(1.0f), initialPetalPosition);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	small_sphere.Draw(shader);

	//daca o ajuns pana sus de la 0 => schiumbaum directia si scadem pana la 0
	if (sphereDirection == 0) {//directia in sus
		if (sphereStep < 1.0f) {
			sphereStep += 0.1f;
		}
		else {
			sphereDirection = 1;
		}
	}
	else {//directia in jos
		if (sphereStep > 0.0f) {
			sphereStep -= 0.1f;
		}
		else {
			sphereDirection = 0;
		}
	}


	//animatie torus sphere 1 & 2
	model = glm::translate(glm::mat4(1.0f), initialTorus1Position);
	//model = glm::rotate(model, M_PI * torusStep, glm::vec3(0.0f, 0.0f, 1.0f));
	//model = glm::rotate(model, M_PI * torusStep, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, M_PI * torusStep, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, -initialTorus1Position);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	torus_sphere_1.Draw(shader);

	model = glm::translate(glm::mat4(1.0f), initialTorus2Position);
	model = glm::rotate(model, M_PI * (-torusStep), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, M_PI * (-torusStep), glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::rotate(model, M_PI * (-torusStep), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, -initialTorus2Position);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	torus_sphere_2.Draw(shader);

	if (torusDirection == 0) {//directia in sus
		if (torusStep < 0.05f) {
			torusStep += 0.0005f;
		}
		else {
			torusDirection = 1;
		}
	}
	else {//directia in jos
		if (torusStep > 0.0f) {
			torusStep -= 0.0005f;
		}
		else {
			torusDirection = 0;
		}
	}

	//animatie petale
	for (int i = 0; i < numberOfPetals; i++) {

		if (petals[i].y > -1.2f) {
			petals[i].y -= petalMovement;
		}
		else {
			petals[i].y = initialPetalsY[i];
		}
		model = glm::translate(glm::mat4(1.0f), petals[i]);
		/*
		//rotatia unei petale
		if ((rand() % 10 + 1) % 2 == 0) {
			model = glm::rotate(model, M_PI * starRotation, glm::vec3(1.0f, 1.0f, 1.0f));
			model = glm::translate(model, -petals[i]);//-?}
		}*/
		glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		if (i % 2 == 0)
			pink_petal.Draw(shader);
		else
			purple_petal.Draw(shader);

	}

	//animatie lanterna+lumina
	model = glm::translate(glm::mat4(1.0f), initialPosLanternLight);
	model = glm::translate(glm::mat4(1.0f), glm::vec3(posLanternLight.x, posLanternLight.y + lanternStep * sin(50), posLanternLight.z));
	//model = glm::translate(glm::mat4(1.0f), -initialPosLanternLight);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	updateLanternLightUniform();
	lantern_light.Draw(shader);


	//daca o ajuns pana sus de la 0 => schiumbaum directia si scadem pana la 0
	if (lanternDirection == 0) {//directia in sus
		if (lanternStep < 1.0f) {
			lanternStep += 0.1f;
		}
		else {
			lanternDirection = 1;
		}
	}
	else {//directia in jos
		if (lanternStep > 0.0f) {
			lanternStep -= 0.1f;
		}
		else {
			lanternDirection = 0;
		}
	}

	model = glm::mat4(1.0f);
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	island.Draw(shader);
	//lantern_light.Draw(shader);

	waterShader.useShaderProgram();
	glUniform1f(glGetUniformLocation(waterShader.shaderProgram, "fogDensity"), fogDensity);
	glUniformMatrix3fv(glGetUniformLocation(waterShader.shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(waterShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	waterfall.Draw(waterShader);
}

void renderScene() {

	if (presenting) {
		presentation_cnt++;
		if (presentation_cnt >= presentation_delay) {
			presentation_cnt = 0;
			if (presentationId >= presentationCamera.size()) {
				myCamera.setCameraParams(initial_pos, initial_target, initial_up, initial_pitch, initial_yaw, true);
				presenting = false;
			}
			else {
				cameraFromFile params = presentationCamera.at(presentationId++);
				myCamera.setCameraParams(params.position, params.target, params.up, params.pitch, params.yaw, false);
			}
		}
	}

	depthShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthShader.shaderProgram, "lightSpaceTrMatrix"),
		1,
		GL_FALSE,
		glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key

	if (showDepthMap) {
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		//bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		//bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
			1,
			GL_FALSE,
			glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		//draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
		mySkyBox.Draw(skyboxShader, view, projection);
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	//close GL context and any other GLFW resources
	glfwTerminate();
}

int main(int argc, const char* argv[]) {

	if (!initOpenGLWindow()) {
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initPetals();
	initShaders();
	initSkyBox();
	initUniforms();
	initFBO();

	loadPresentation();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow)) {
		processMovement();
		renderScene();
		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
