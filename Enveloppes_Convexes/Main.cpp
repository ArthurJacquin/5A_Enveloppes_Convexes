#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GLShader.h"

#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "glew32s.lib")	
#pragma comment(lib, "opengl32.lib")
#elif defined(__APPLE__)
#elif defined(__linux__)
#endif

#include <iostream>
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm\gtx\transform.hpp>
#include "Vertex.h"
#include "Input.h"
#include "OpenGLcore.h"
#include "ConvexEnvelope.h"
#include "Algos.h"

#include "imgui.h"
#include "examples\\imgui_impl_opengl3.h"
#include "examples\\imgui_impl_glfw.h"

#include "imgui.cpp"
#include "examples\\imgui_impl_glfw.cpp"
#include "examples\\imgui_impl_opengl3.cpp"
#include "imgui_draw.cpp"
#include "imgui_widgets.cpp"
#include "imgui_demo.cpp"

#include "Color.h"
#include "Mesh.h"
#include "Triangulation.h"
#include "ConvexEnvelope3D.h"

#pragma region Variables

const char* glsl_version = "#version 420";

//Variables globales
GLShader BasicShader;
GLuint VAO;
uint32_t VBOCurrent;
Input input;

//tableau de positions du tableau en cours
std::vector<Vertex> pointsCloud;
std::vector<ConvexEnvelope> convexEnv;
std::vector<ConvexEnvelope3D> convexEnv3D;
Triangulation triangulation;

std::vector<int> selectedPoints;
int selectedPointId;

int width = 1600;
int height = 800;

bool enableWireframe;
extern bool enable3DViewport;
bool enableNormal;

int enableNormalLocation;
int enable3DViewportLocation;
int modelMatrixLocation;
int viewMatrixLocation;
int projectionMatrixLocation;
int cameraPos_location;

#pragma endregion

#pragma region Rendering_Functions

//Init scene rendering
bool Initialise() {

	//Init GLEW
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		std::cout << "erreur d'initialisation de GLEW!"
			<< std::endl;
	}
	
	std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;
	
	//Init shaders
	BasicShader.LoadVertexShader("Basic.vs.glsl");
	BasicShader.LoadFragmentShader("Basic.fs.glsl");
	BasicShader.Create();
	
	//Init Program
	GLuint64 BasicProgram = BasicShader.GetProgram();
	glUseProgram(BasicProgram);
	
	modelMatrixLocation = glGetUniformLocation(BasicProgram, "u_modelMatrix");
	viewMatrixLocation = glGetUniformLocation(BasicProgram, "u_viewMatrix");
	projectionMatrixLocation = glGetUniformLocation(BasicProgram, "u_projectionMatrix");
	
	enable3DViewportLocation = glGetUniformLocation(BasicProgram, "u_enable3DViewport");
	enableNormalLocation = glGetUniformLocation(BasicProgram, "u_enabledNormal");
	cameraPos_location = glGetUniformLocation(BasicProgram, "u_camPos");
	
	return true;
}

//Update the VBO
void updateVBO()
{
	//Cr�ation VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBOCurrent);
	
	//Position
	int loc_position = 0;
	glVertexAttribPointer(loc_position, 3, GL_DOUBLE, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(loc_position);
	
	//Color
	int loc_color = 1;
	glVertexAttribPointer(loc_color, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, r));
	glEnableVertexAttribArray(loc_color);

	//Normals
	int loc_normal = 2;
	glVertexAttribPointer(loc_normal, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(loc_normal);
}

//Delete everything
void Terminate() 
{
	BasicShader.Destroy();
	DeleteBufferObject(VBOCurrent);
	glDeleteVertexArrays(1, &VAO);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

//REnder the scene
void Display(GLFWwindow* window)
{
	glfwGetWindowSize(window, &width, &height);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Desactive le "scissoring"
	glDisable(GL_SCISSOR_TEST);
	// Defini le viewport en pleine fenetre
	glViewport(0, 0, width, height);

	glLineWidth(1.f);
	glEnable(GL_PROGRAM_POINT_SIZE);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	//Cr�ation VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	input.computeMatricesFromInputs(window, width, height);

	glm::mat4 view = input.getViewMatrix();
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &view[0][0]);

	glm::mat4 projection = input.getProjectionMatrix();
	glm::mat4 ortho = glm::ortho(-1, 1, -1, 1, -1, 1);

	if(enable3DViewport)
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projection[0][0]);
	else
		glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &ortho[0][0]);

	//Wireframe
	if (enableWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	//Shading
	glUniform1f(enableNormalLocation, enableNormal);
	glUniform1f(enable3DViewportLocation, enable3DViewport);
	glUniform3fv(cameraPos_location, 1, &input.getPosition()[0]);

	//Create VBO with vertices
	VBOCurrent = CreateBufferObject(BufferType::VBO, sizeof(Vertex) * pointsCloud.size(), pointsCloud.data());
	updateVBO();
	
	//Draw points
	glDrawArrays(GL_POINTS, 0, pointsCloud.size());

	//Draw convex Envelope
	for (int i = 0; i < convexEnv.size(); i++)
	{
		VBOCurrent = convexEnv[i].GetVBO();
		updateVBO();

		glDrawArrays(GL_LINE_LOOP, 0, convexEnv[i].GetPointsCount());
	}


	//Draw convex Envelope 3D
	for (int i = 0; i < convexEnv3D.size(); i++)
	{
		for (size_t j = 0; j < convexEnv3D[i].GetTrianglesCount(); j++)
		{
			VBOCurrent = convexEnv3D[i].GetTriangles()[j].GetVBO();
			updateVBO();

			glDrawArrays(GL_TRIANGLES, 0, 3);
		}
	}

	//Draw triangulation
	for (int i = 0; i < triangulation.edge.size(); i++)
	{
		VBOCurrent = triangulation.edge[i].GetVBO();
		updateVBO();

		glCullFace(GL_FRONT_AND_BACK);
		glDrawArrays(GL_LINES, 0, 2);
	}

	//D�sactivation des buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Init UI
void InitialiseGUI(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsClassic();
}

//Render UI
void displayGUI()
{
	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::SetNextWindowSize(ImVec2(300, 780));
	// render your GUI
	ImGui::Begin("Bezier", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	ImGui::TextColored(ImVec4(0.9, 0.1, 0.1, 1.0), "  Enveloppes convexes et triangulation ");
	ImGui::Text("");

	//-------------------------------------------------------------------------Keys Guide---------------------------------------------------------
	ImGui::Separator();
	ImGui::Text("");
	ImGui::Text("  Select a point :");
	ImGui::Text("  right click");
	ImGui::Text("");

	//-------------------------------------------------------------------------Point generation---------------------------------------------------------
	ImGui::Separator();
	ImGui::Text("            Generate points    ");
	ImGui::Text("");

	static int taille = 10;
	ImGui::Text("Number of points :");
	ImGui::InputInt("", &taille);

	if (ImGui::Button("Generate 2D points cloud"))
	{
		pointsCloud.clear();
		for (int i = 0; i < taille; ++i)
		{
			double x = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.5f)) - 0.5f;
			double y = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 1.8f)) - 0.9f;

			pointsCloud.push_back(Vertex(x, y, 0));
		}
	}
	if (ImGui::Button("Generate 3D points cloud"))
	{
		pointsCloud.clear();
		for (int i = 0; i < taille; ++i)
		{
			double x = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 0.4f)) - 0.2f;
			double y = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 0.4f)) - 0.2f;
			double z = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / 0.4f)) - 0.2f;

			pointsCloud.push_back(Vertex(x, y, z));
		}
	}
	ImGui::Text("");

	//-------------------------------------------------------------------------Convex envelope---------------------------------------------------------
	ImGui::Separator();
	ImGui::Text("            Convex Hull    ");
	ImGui::Text("");
	if (ImGui::Button("Jarvis"))
	{
		convexEnv.push_back(Jarvis(pointsCloud));
	}
	if (ImGui::Button("GrahamScan"))
	{
		convexEnv.push_back(GrahamScan(pointsCloud));
	}
	if (ImGui::Button("3D convex hull"))
	{
		convexEnv3D.push_back(Envelope3D(pointsCloud));
	}
	ImGui::Text("");

	//-------------------------------------------------------------------------Triangulation---------------------------------------------------------
	ImGui::Separator();
	ImGui::Text("            Triangulation    ");
	ImGui::Text("");
	if (ImGui::Button("Incremental"))
	{
		triangulation = triangulateIncremental(pointsCloud);
	}
	if (ImGui::Button("Flipping edges from incremental"))
	{
		triangulation = triangulateDelaunay(pointsCloud);
	}
	if (ImGui::Button("Delaunay"))
	{
		triangulation = coreDelaunay(pointsCloud);
	}
	if (ImGui::Button("Delete delaunay point"))
	{
		std::sort(selectedPoints.begin(), selectedPoints.end(), greater<int>());
		for (size_t i = 0; i < selectedPoints.size(); i++)
		{
			removeCoreDelaunay(triangulation, &pointsCloud[selectedPoints[i]]);
		}
		for (size_t i = 0; i < selectedPoints.size(); i++)
		{
			pointsCloud[selectedPoints[i]].setColor(Color(0, 0, 0));
		}
		selectedPoints.clear();
	}
	if (ImGui::Button("2D Voronoi"))
	{
		triangulation = voronoiDiagram(pointsCloud);
	}
	ImGui::Text("");

	//-------------------------------------------------------------------------Visualisation tools---------------------------------------------------------
	ImGui::Separator();
	ImGui::Text("");
	if (ImGui::Button("Wireframe"))
	{
		enableWireframe = !enableWireframe;
	}
	ImGui::SameLine();
	if (ImGui::Button("Normales"))
	{
		enableNormal = !enableNormal;
	}
	ImGui::Text("");
	if (ImGui::Button("3D Viewport"))
	{
		enable3DViewport = !enable3DViewport;
		updateVBO();
	}
	ImGui::Text("");
	if (ImGui::Button("Delete all curves"))
	{
		pointsCloud.clear();
		convexEnv.clear();
		triangulation.clear();
		convexEnv3D.clear();
		selectedPoints.clear();
	}

	ImGui::End();

	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

//Main
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Enveloppes Convexes", NULL, NULL);
	InitialiseGUI(window);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// toutes nos initialisations vont ici
	Initialise();
	

	//Input
	glfwSetMouseButtonCallback(window, &input.mouse_button_callback);
	glfwSetKeyCallback(window, &input.keyboard_button_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwGetFramebufferSize(window, &width, &height);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		Display(window);

		displayGUI();

		glfwSwapBuffers(window);
		/* Poll for and process events */
		glfwPollEvents();

	}

	// ne pas oublier de liberer la memoire etc...
	Terminate();

	glfwTerminate();
	return 0;
}

#pragma endregion