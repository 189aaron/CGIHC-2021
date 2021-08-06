#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"


// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"

// Function prototypes
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);
void DoMovement();
void animacion();

// Window dimensions
const GLuint WIDTH = 1024, HEIGHT = 720;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera(glm::vec3(5.0f, 3.0f, 35.0f));
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
float range = 0.0f;
float rot = 90.0f;


// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
glm::vec3 PosIni(-25.0f, 0.0f, -15.0f);
glm::vec3 PosIniAuto(-15.0f, 0.0f, -15.0f);
glm::vec3 casa_2(35.0f, 0.0f, 0.0f);
glm::vec3 casa_3(9.0f, 0.0f, 55.0f);
glm::vec3 casa_4(44.0f, 0.0f, 55.0f);

glm::vec3 PosIni1(0.0f, 0.0f, 0.0f);//para mover al auto
glm::vec3 PosIniAuto1(0.0f, 0.0f, 15.0f);//para mover al auto
bool active;


// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Keyframes
float posX =PosIni.x, posY = PosIni.y, posZ = PosIni.z, rotRodIzq = 0;

#define MAX_FRAMES 9
int i_max_steps = 190;
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float incX;		//Variable para IncrementoX
	float incY;		//Variable para IncrementoY
	float incZ;		//Variable para IncrementoZ
	float rotRodIzq;
	float rotInc;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(posX,posY,posZ),
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};

glm::vec3 LightP1;

//Animación del coche
float movKitX = 0.0;
float movKitZ = 0.0;
float rotKit = 0.0;//orientación del auto
float rotKit_izq = 0.0;//giro de las ruedas izq
float rotKit_der = 0.0;//giro de las ruedas derechas
float movKitX1 = 0.0;
float movKitZ1 = 0.0;

float rotKit1 = 0.0;//orientación de la casa 1
float rotKit2 = 0.0;//orientación de la casa 2
float rotKit3 = 180.0;//orientación de la casa 3
float rotKit4 = 180.0;//orientación de la casa 4

bool circuito = false;
bool recorrido1 = true;
bool recorrido2 = false;
bool recorrido3 = false;
bool recorrido4 = false;
bool recorrido5 = false;

float abrir1 = 0.0;
float abrir2 = 0.0;
float abrir3 = 0.0;
float abrir4 = 0.0f;

float aire = 0.0f;

float mov_vent = 0.0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);
	
	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;
	
	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	

	FrameIndex++;
}

void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;

}

void interpolation(void)
{

	KeyFrame[playIndex].incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	KeyFrame[playIndex].incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	KeyFrame[playIndex].incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;
	
	KeyFrame[playIndex].rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;

}




int main()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Casa", nullptr, nullptr);

	if (nullptr == window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	printf("%f\n", glfwGetTime());

	// GLFW Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (GLEW_OK != glewInit())
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return EXIT_FAILURE;
	}

	// Define the viewport dimensions
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Setup and compile our shaders

	Shader shader("Shaders/modelLoading.vs", "Shaders/modelLoading.frag");
	Shader lightingShader("Shaders/lighting.vs", "Shaders/lighting.frag");
	Shader lampShader("Shaders/lamp.vs", "Shaders/lamp.frag");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");

	Model Carroseria((char*)"Models/Carro/Carroseria.obj");
	Model LLanta((char*)"Models/Carro/Wheel.obj");
	Model pasto((char *)"Models/pasto/pasto.obj");
	Model arbusto((char *)"Models/arbusto/arbusto.obj");
	Model calle((char *)"Models/camino/calle.obj");
	

	Model cama((char *)"Models/cama/Single_Bed.obj");
	Model fogata((char *)"Models/fogata/Campfire.obj");
	Model casa_aa((char *)"Models/villaf/villaF1.obj");
	Model casa_aa_c((char *)"Models/villaf/villaF_cristales.obj");

	Model perrito((char *)"Models/perro/cherry.obj");
	Model cerca((char *)"Models/villaf/cerca.obj");
	Model camastro((char *)"Models/sunbed/camastrof.obj");
	Model mesa((char *)"Models/mesaf/mesaf.obj");
	Model silla((char *)"Models/mesaf/silla.obj");
	Model mesa_estancia((char *)"Models/mesa_estancia/mesa.obj");
	Model alacena((char *)"Models/cocina/almacen.obj");
	
	Model puerta((char *)"Models/villaf/puerta.obj");
	Model puerta2((char *)"Models/villaf/puerta2.obj");
	Model puerta3((char *)"Models/villaf/puerta3.obj");
	Model puerta4((char *)"Models/villaf/puerta4.obj");
	
	Model ventanales((char *)"Models/villaf/ventanales.obj");
	Model ventanales2((char *)"Models/villaf/ventanales2.obj");
	Model ventanales3((char *)"Models/villaf/ventanales3.obj");
	Model ventanales4((char *)"Models/villaf/ventanales4.obj");
	
	// Build and compile our shader program

	//Inicialización de KeyFrames
	
	for(int i=0; i<MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].incX = 0;
		KeyFrame[i].incY = 0;
		KeyFrame[i].incZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].rotInc = 0;
	}



	// Set up vertex data (and buffer(s)) and attribute pointers
	GLfloat vertices[] =
	{
		// Positions            // Normals              // Texture Coords
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  0.0f,  1.0f,  	1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,

		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,
		0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,
		0.5f,  0.5f, -0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f
	};


	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};


	GLuint indices[] =
	{  // Note that we start from 0!
		0,1,2,3,
		4,5,6,7,
		8,9,10,11,
		12,13,14,15,
		16,17,18,19,
		20,21,22,23,
		24,25,26,27,
		28,29,30,31,
		32,33,34,35
	};

	// Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	// First, set the container's VAO (and VBO)
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	// Normals attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Texture Coordinate attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	// Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	// We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Set the vertex attributes (only position data for the lamp))
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid *)0); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	//SkyBox
	GLuint skyboxVBO, skyboxVAO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1,&skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices),&skyboxVertices,GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/right.tga");
	faces.push_back("SkyBox/left.tga");
	faces.push_back("SkyBox/top.tga");
	faces.push_back("SkyBox/bottom.tga");
	faces.push_back("SkyBox/back.tga");
	faces.push_back("SkyBox/front.tga");
	
	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	lightingShader.Use();
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.diffuse"), 0);
	glUniform1i(glGetUniformLocation(lightingShader.Program, "material.specular"), 1);

	glm::mat4 projection = glm::perspective(camera.GetZoom(), (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT, 0.1f, 1000.0f);

	// Game loop
	while (!glfwWindowShouldClose(window))
	{

		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		DoMovement();
		animacion();


		// Clear the colorbuffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// Use cooresponding shader when setting uniforms/drawing objects
		lightingShader.Use();
		GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
		glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);
		// == ==========================
		// Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
		// the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
		// by defining light types as classes and set their values in there, or by using a more efficient uniform approach
		// by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
		// == ==========================
		// Directional light
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.8f, 0.8f, 0.8f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.5f, 0.5f, 0.5f);


		// Point light 1
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"), LightP1.x, LightP1.y, LightP1.z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"), LightP1.x, LightP1.y, LightP1.z);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.032f);



		// Point light 2
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].diffuse"), 1.0f, 1.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[1].specular"), 1.0f, 1.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[1].quadratic"), 0.032f);

		// Point light 3
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].diffuse"), 0.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[2].specular"), 0.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[2].quadratic"), 0.032f);

		// Point light 4
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].diffuse"), 1.0f, 0.0f, 1.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[3].specular"), 1.0f, 0.0f, 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[3].quadratic"), 0.032f);

		// SpotLight
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"), camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

		// Set material properties
		glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

		// Create camera transformations
		glm::mat4 view;
		view = camera.GetViewMatrix();

		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
		GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
		GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		glBindVertexArray(VAO);
		glm::mat4 tmp = glm::mat4(1.0f); //Temp

		//Carga de modelos
		
		view = camera.GetViewMatrix();
		glm::mat4 model(1);
		tmp = model = glm::translate(model, glm::vec3(0, 1, 0));

	
		//Carga de modelo del auto
		//Carroceria
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniAuto1 + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		Carroseria.Draw(lightingShader);

		//Llanta Delantera Der
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniAuto1 + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(1.7f, 0.5f, 2.6f));;
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		//model = glm::rotate(model, glm::radians(rotKit_der), glm::vec3(0.0f, 0.0f, 1.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LLanta.Draw(lightingShader);

		//Llanta Trasera Der
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniAuto1 + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(1.7f, 0.5f, -2.9f));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LLanta.Draw(lightingShader);


		//Llanta Delantera Izq
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniAuto1 + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0, 1.0, 0.0));
		model = glm::translate(model, glm::vec3(-1.7f, 0.5f, 2.6f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		//model = glm::rotate(model, glm::radians(rotKit_izq), glm::vec3(0.0, 0.0, 1.0));//vista de frente
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LLanta.Draw(lightingShader);

		//Llanta Trasera Izq
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniAuto1 + glm::vec3(movKitX, 0, movKitZ));
		model = glm::rotate(model, glm::radians(rotKit), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::translate(model, glm::vec3(-1.7f, 0.8f, -2.9f));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0));
		model = glm::scale(model, glm::vec3(0.02f, 0.02f, 0.02f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		LLanta.Draw(lightingShader);

		glBindVertexArray(0);

		//camino
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(-17.0f, 0.0f, 37.5f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(7.6f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		calle.Draw(lightingShader);

		//Carga de modelos
		//---------------------------------------------------------------------------------------------
		//---------------------------------------CASA 1------------------------------------------------
		//---------------------------------------------------------------------------------------------
		//Villa F
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa.Draw(lightingShader);

		//Villa F cerca
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cerca.Draw(lightingShader);

		//casa cristales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa_c.Draw(lightingShader);

		//ventanales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales.Draw(lightingShader);

		//ventanales 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales2.Draw(lightingShader);

		//ventanales 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales3.Draw(lightingShader);

		//ventanales 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales4.Draw(lightingShader);

		//puerta
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta.Draw(lightingShader);

		//puerta 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta2.Draw(lightingShader);

		//puerta 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta3.Draw(lightingShader);

		//puerta4 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta4.Draw(lightingShader);

		//fogata
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(24, -0.4, 22));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		fogata.Draw(lightingShader);

		//cama cuarto principal
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(10.2, 2.7, 14));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cama.Draw(lightingShader);

		//cama cuarto del fondo
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(21.1, 2.7, 11));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cama.Draw(lightingShader);

		//camastro 1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(6, 0, 24));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//camastro 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(6, 0, 22.7));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//camastro 3 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(6, 0, 21.4));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//camastro 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(6, 0, 20.1));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//mesa exterior
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16.5, 0, 23));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mesa.Draw(lightingShader);

		//mesa estancia
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(21, -0.4, 23));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mesa_estancia.Draw(lightingShader);

		//silla exterior 1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16, 0, 23));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16, 0, 23.8));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16, 0, 24.6));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16, 0, 20.9));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16, 0, 21.5));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(16, 0, 22.3));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//mesa interior
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(13.5, 0, 15));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mesa.Draw(lightingShader);

		//silla interior 1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(13.5, 0, 15.2));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(14, 0, 15.2));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(14.5, 0, 15.2));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);


		//silla interior 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12, 0, 15.5));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(12.5, 0, 15.5));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(13, 0, 15.5));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);


		//alacena
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(20.8, 0, 10.5));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f, 0.9f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		alacena.Draw(lightingShader);

		//alacena
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(30, 0, 10.5));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f, 0.9f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		alacena.Draw(lightingShader); 

		//Perrito
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, PosIniAuto1 + glm::vec3(30, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit1), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		perrito.Draw(lightingShader);

		//---------------------------------------------------------------------------------------------
		//---------------------------------------CASA 2------------------------------------------------
		//---------------------------------------------------------------------------------------------
		//Villa F
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit2), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa.Draw(lightingShader);

		//Villa F cerca
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit2), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cerca.Draw(lightingShader);

		//casa cristales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit2), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa_c.Draw(lightingShader);

		//ventanales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10 + mov_vent, 0, 10));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales.Draw(lightingShader);

		//ventanales 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 +  glm::vec3(10 + mov_vent, 0, 10));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales2.Draw(lightingShader);

		//ventanales 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10 + mov_vent, 0, 10));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales3.Draw(lightingShader);

		//ventanales 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10 + mov_vent, 0, 10));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales4.Draw(lightingShader);

		//puerta
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(abrir1), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta.Draw(lightingShader);

		//puerta 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(abrir2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta2.Draw(lightingShader);

		//puerta 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(abrir3), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta3.Draw(lightingShader);

		//puerta4 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(abrir4), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta4.Draw(lightingShader);

		//fogata
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(24, -0.4, 22));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		fogata.Draw(lightingShader);

		//cama cuarto principal
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(10.2, 2.7, 14));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cama.Draw(lightingShader);

		//cama cuarto del fondo
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(21.1, 2.7, 11));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cama.Draw(lightingShader);

		//camastro 1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(6, 0, 24));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//camastro 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(6, 0, 22.7));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//camastro 3 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(6, 0, 21.4));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//camastro 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(6, 0, 20.1));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		camastro.Draw(lightingShader);

		//mesa exterior
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(16.5, 0, 23));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mesa.Draw(lightingShader);

		//mesa estancia
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(21, -0.4, 23));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mesa_estancia.Draw(lightingShader);

		//silla exterior 1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(16, 0, 23));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(16, 0, 23.8));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(16, 0, 24.6));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(16, 0, 20.9));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(16, 0, 21.5));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla exterior 6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(16, 0, 22.3));
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//mesa interior
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(13.5, 0, 15));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		mesa.Draw(lightingShader);

		//silla interior 1
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(13.5, 0, 15.2));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(14, 0, 15.2));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(14.5, 0, 15.2));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);


		//silla interior 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(12, 0, 15.5));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 5
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(12.5, 0, 15.5));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);

		//silla interior 6
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(13, 0, 15.5));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		silla.Draw(lightingShader);


		//alacena
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(20.8, 0, 10.5));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f, 0.9f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		alacena.Draw(lightingShader);

		//alacena
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_2 + glm::vec3(30, 0, 10.5));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		model = glm::scale(model, glm::vec3(1.0f, 0.9f, 1.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		alacena.Draw(lightingShader);

	
		//---------------------------------------------------------------------------------------------
		//---------------------------------------CASA 3------------------------------------------------
		//---------------------------------------------------------------------------------------------
		//Villa F
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa.Draw(lightingShader);

		//Villa F cerca
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cerca.Draw(lightingShader);

		//casa cristales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa_c.Draw(lightingShader);

		//ventanales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales.Draw(lightingShader);

		//ventanales 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales2.Draw(lightingShader);

		//ventanales 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales3.Draw(lightingShader);

		//ventanales 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales4.Draw(lightingShader);

		//puerta
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta.Draw(lightingShader);

		//puerta 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta2.Draw(lightingShader);

		//puerta 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta3.Draw(lightingShader);

		//puerta4 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta4.Draw(lightingShader);

		////fogata
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(24, -0.4, 22));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//fogata.Draw(lightingShader);

		////cama cuarto principal
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(10.2, 2.7, 14));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//cama.Draw(lightingShader);

		////cama cuarto del fondo
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(21.1, 2.7, 11));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//cama.Draw(lightingShader);

		////camastro 1
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(6, 0, 24));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//camastro.Draw(lightingShader);

		////camastro 2
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(6, 0, 22.7));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//camastro.Draw(lightingShader);

		////camastro 3 
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(6, 0, 21.4));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//camastro.Draw(lightingShader);

		////camastro 4
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(6, 0, 20.1));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//camastro.Draw(lightingShader);

		////mesa exterior
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(16.5, 0, 23));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//mesa.Draw(lightingShader);

		////mesa estancia
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(21, -0.4, 23));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//mesa_estancia.Draw(lightingShader);

		////silla exterior 1
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(16, 0, 23));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla exterior 2
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(16, 0, 23.8));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla exterior 3
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(16, 0, 24.6));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla exterior 4
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(16, 0, 20.9));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla exterior 5
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(16, 0, 21.5));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla exterior 6
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(16, 0, 23.3));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////mesa interior
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(13.5, 0, 15));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//mesa.Draw(lightingShader);

		////silla interior 1
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(13.5, 0, 15.2));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla interior 2
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(14, 0, 15.2));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla interior 3
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(14.5, 0, 15.2));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);


		////silla interior 4
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(12, 0, 15.5));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla interior 5
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(12.5, 0, 15.5));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);

		////silla interior 6
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(13, 0, 15.5));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//silla.Draw(lightingShader);


		////alacena
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(20.8, 0, 10.5));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(1.0f, 0.9f, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//alacena.Draw(lightingShader);

		////alacena
		//view = camera.GetViewMatrix();
		//model = glm::mat4(1);
		//model = glm::translate(model, casa_3 + glm::vec3(30, 0, 10.5));
		//model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0));
		//model = glm::scale(model, glm::vec3(1.0f, 0.9f, 1.0f));
		//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		//alacena.Draw(lightingShader);

//---------------------------------------------------------------------------------------------
		//---------------------------------------CASA 3------------------------------------------------
		//---------------------------------------------------------------------------------------------
		//Villa F
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa.Draw(lightingShader);

		//Villa F cerca
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cerca.Draw(lightingShader);

		//casa cristales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa_c.Draw(lightingShader);

		//ventanales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales.Draw(lightingShader);

		//ventanales 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales2.Draw(lightingShader);

		//ventanales 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales3.Draw(lightingShader);

		//ventanales 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales4.Draw(lightingShader);

		//puerta
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta.Draw(lightingShader);

		//puerta 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta2.Draw(lightingShader);

		//puerta 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta3.Draw(lightingShader);

		//puerta4 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_3 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit3), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta4.Draw(lightingShader);

		//---------------------------------------------------------------------------------------------
		//---------------------------------------CASA 4------------------------------------------------
		//---------------------------------------------------------------------------------------------
		//Villa F
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa.Draw(lightingShader);

		//Villa F cerca
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		cerca.Draw(lightingShader);

		//casa cristales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		casa_aa_c.Draw(lightingShader);

		//ventanales
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales.Draw(lightingShader);

		//ventanales 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales2.Draw(lightingShader);

		//ventanales 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales3.Draw(lightingShader);

		//ventanales 4
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10 + mov_vent, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		ventanales4.Draw(lightingShader);

		//puerta
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta.Draw(lightingShader);

		//puerta 2
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta2.Draw(lightingShader);

		//puerta 3
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta3.Draw(lightingShader);

		//puerta4 
		view = camera.GetViewMatrix();
		model = glm::mat4(1);
		model = glm::translate(model, casa_4 + glm::vec3(10, 0, 10));
		model = glm::rotate(model, glm::radians(rotKit4), glm::vec3(0.0f, 1.0f, 0.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		puerta4.Draw(lightingShader);

		// Also draw the lamp object, again binding the appropriate shader
		lampShader.Use();
		// Get location objects for the matrices on the lamp shader (these could be different on a different shader)
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		// Set matrices
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// Draw skybox as last
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		SkyBoxshader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));	// Remove any translation component of the view matrix
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(SkyBoxshader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default

		glBindVertexArray(lightVAO);
		for (GLuint i = 0; i < 4; i++)
		{
			model = glm::mat4(1);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);


		// Swap the screen buffers
		glfwSwapBuffers(window);
	}




	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &skyboxVAO);
	glDeleteBuffers(1, &skyboxVBO);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();

	return 0;
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (keys[GLFW_KEY_L])
	{
		if (play == false && (FrameIndex > 1))
		{

			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}

	}

	if (keys[GLFW_KEY_K])
	{
		if (FrameIndex<MAX_FRAMES)
		{
			saveFrame();
		}

	}


	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keys[key] = false;
		}
	}

	if (keys[GLFW_KEY_SPACE])
	{
		active = !active;
		if (active)
			LightP1 = glm::vec3(1.0f, 0.0f, 0.0f);
		else
			LightP1 = glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos)
{

	if (firstMouse)
	{
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	GLfloat xOffset = xPos - lastX;
	GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
	//animacipon del auto
	if (keys[GLFW_KEY_I])
	{
		circuito = true;
	}

	if (keys[GLFW_KEY_O])
	{
		circuito = false;
	}

	//animacion de las puertas
	if (keys[GLFW_KEY_1])
	{
		if (abrir1 >= 0) {
			abrir1 -= 0.5f;
		}
	}
	if (keys[GLFW_KEY_2])
	{
		if (abrir1 <= 90) {
			abrir1 += 0.5f;
		}
	}
	//animacion de las puertas
	if (keys[GLFW_KEY_3])
	{
		if (abrir2 >= 0) {
			abrir2 -= 0.5f;
		}
	}
	if (keys[GLFW_KEY_4])
	{
		if (abrir2 <= 90) {
			abrir2 += 0.5f;
		}
	}//animacion de las puertas
	if (keys[GLFW_KEY_5])
	{
		if (abrir3 >= 0) {
			abrir3 -= 0.5f;
		}
	}
	if (keys[GLFW_KEY_6])
	{
		if (abrir3 <= 90) {
			abrir3 += 0.5f;
		}
	}//animacion de las puertas
	if (keys[GLFW_KEY_7])
	{
		if (abrir4 >= 0) {
			abrir4 -= 0.5f;
		}
	}
	if (keys[GLFW_KEY_8])
	{
		if (abrir4 <= 90) {
			abrir4 += 0.5f;
		}
	}

	if (keys[GLFW_KEY_9])
	{
		if (aire >= -3) {
			aire -= 0.5f;
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (aire <= 3) {
			aire += 0.5f;
		}
	}

	if (keys[GLFW_KEY_Q])
	{
		if (mov_vent >= 0.1) {
			mov_vent -= 0.1f;
			//printf("mov_vent %f\n", mov_vent);
		}
	}
	if (keys[GLFW_KEY_E])
	{
		if (mov_vent <= 1.5) {
			mov_vent += 0.1f;
			//printf("mov_vent %f\n", mov_vent);
		}
	}



	// Camera controls
	if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);

	}

	if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);


	}

	if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
	{
		camera.ProcessKeyboard(LEFT, deltaTime);


	}

	if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}






}

void animacion()
{

	//Movimiento del coche
	if (circuito)
	{

		if (recorrido1)
		{
			rotKit = 90;
			movKitX += 0.3f;
			if (movKitX > 30)
			{
				recorrido1 = false;
				recorrido2 = true;

			}
		}
		if (recorrido2)
		{
			rotKit = 0;
			movKitZ += 0.3f;
			if (movKitZ > 30)
			{
				recorrido2 = false;
				recorrido3 = true;
			}
		}
		if (recorrido3)
		{
			rotKit = 270;
			movKitX -= 0.3f;
			if (movKitX < 0)
			{
				recorrido3 = false;
				recorrido4 = true;
			}
		}
		if (recorrido4)
		{
			rotKit = 180;
			movKitZ -= 0.3f;
			if (movKitZ < 0)
			{
				recorrido4 = false;
				recorrido5 = true;
			}
		}
		if (recorrido5)
		{
			rotKit = 90;
			movKitZ += 0.3f;
			if (movKitZ > 0)
			{
				recorrido5 = false;
				recorrido1 = true;
			}
		}


	}
}