// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#ifndef GLEW_DEFINED
#define GLEW_DEFINED
#include <GL/glew.h>
#endif

// Include GLFW
#ifndef GLFW_DEFINED
#define GLFW_DEFINED
#include <GL/glfw.h>
#endif

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <common/shader.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <AntTweakBar.h>
//#include "TRIModel.h"
#include "CDT.h"

using namespace std;
using namespace glm;

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 700, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	float dx = 0;			// ¬ö¿ý¦ì²¾¶q
	float dy = 0;

	vec3 light1 = vec3(0, 0, 0);
	vec3 light2 = vec3(0, 0, 0);
	vec3 light3 = vec3(0, 0, 0);

	bool pressF4 = false;
	bool pressF5 = false;
	bool pressF6 = false;
	bool pressF7 = false;
	bool pressF8 = false;
	bool mousePressR = false;
	bool CDTdone = false;
	bool pruneDone = false;

	vector<float> clickPoint;
	Mesh mesh;

	vector<vec3> vertices;
	vector<vec3> normals;
	vector<vec3> colors;

	vec3 gEulerRotation;
	quat gQuaternion;
	bool gNormalizeQuaternion;

	GLuint vertexbuffer;
	GLuint normalbuffer;
	GLuint colorbuffer;

	// Initialize the GUI
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(1024, 700);
	TwBar * EulerGUI = TwNewBar("Euler settings");
	TwBar * QuaternionGUI = TwNewBar("Quaternion settings");
	TwSetParam(QuaternionGUI, NULL, "position", TW_PARAM_CSTRING, 1, "808 16");

	TwAddVarRW(EulerGUI, "Euler X", TW_TYPE_FLOAT, &gEulerRotation.x, "step=0.01");
	TwAddVarRW(EulerGUI, "Euler Y", TW_TYPE_FLOAT, &gEulerRotation.y, "step=0.01");
	TwAddVarRW(EulerGUI, "Euler Z", TW_TYPE_FLOAT, &gEulerRotation.z, "step=0.01");

	TwAddVarRW(QuaternionGUI, "Quaternion", TW_TYPE_QUAT4F, &gQuaternion, "showval=true open=true ");
	TwAddVarRW(QuaternionGUI, "Normalize", TW_TYPE_BOOL8, &gNormalizeQuaternion, "help='Will renormalize the quaternion each frame so that the quaternion is always valid'");

	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetMouseButtonCallback((GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetMousePosCallback((GLFWmouseposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetMouseWheelCallback((GLFWmousewheelfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback((GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback((GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar

	glfwSetWindowTitle( "ICG Final - Teddy System" );

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	glfwSetMousePos(1024/2, 700/2);

	// Dark background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "FlatShading.vertexshader", "FlatShading.fragmentshader" );
	GLuint programID2 = LoadShaders( "GouraudShading.vertexshader", "GouraudShading.fragmentshader" );
	GLuint programID3 = LoadShaders( "PhongShading.vertexshader", "PhongShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Get a handle for our buffers, connects to shader
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
	GLuint vertexColorID = glGetAttribLocation(programID, "vertexColor");

	// Get a handle for our "LightPosition" uniform
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint LightID2 = glGetUniformLocation(programID, "LightPosition2_worldspace");
	GLuint LightID3 = glGetUniformLocation(programID, "LightPosition3_worldspace");

	// Creat vertex buffer
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &colorbuffer);
	glGenBuffers(1, &normalbuffer);

	// Use our shader
	glUseProgram(programID);

	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (glfwGetKey( GLFW_KEY_DEL ) == GLFW_PRESS && CDTdone){
			dx = 0;
			dy = 0;
			vertices.clear();
			normals.clear();
			colors.clear();
			mesh.release();
			CDTdone = false;
			pruneDone = false;
		}
		if (glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS){
			if(!CDTdone){
				int xpos, ypos;
				glfwGetMousePos( &xpos, &ypos );
				if(clickPoint.size() == 0){
					clickPoint.push_back((float)(xpos-512));
					clickPoint.push_back((float)(350-ypos));
					clickPoint.push_back(0.f);
					vertices.push_back(vec3(xpos-512, 350-ypos, 0));
					normals.push_back(vec3(0, 0, 1));
					colors.push_back(vec3(255, 255, 255));
				} else{
					double distance = pow(pow((double)(xpos-512) - clickPoint[clickPoint.size()-3], 2) + pow((double)(350-ypos) - clickPoint[clickPoint.size()-2], 2), 0.5);
					if(distance > 5.0){
						if(vertices.size() % 2 == 0){
							vertices.push_back(vec3(vertices[vertices.size() - 1].x, vertices[vertices.size() - 1].y, 0));
							normals.push_back(vec3(0, 0, 1));
							colors.push_back(vec3(255, 255, 255));
						}
						vertices.push_back(vec3(xpos-512, 350-ypos, 0));
						normals.push_back(vec3(0, 0, 1));
						colors.push_back(vec3(255, 255, 255));
						if(distance > 30.0){
							clickPoint.push_back((float)(xpos-512));
							clickPoint.push_back((float)(350-ypos));
							clickPoint.push_back(0.f);
						}

						// Update the whole buffer and load it into a VBO
						glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
						glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
						glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
						glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
						glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
					}
				}
			}
		}
		if (glfwGetMouseButton( GLFW_MOUSE_BUTTON_LEFT ) == GLFW_RELEASE){
			if(vertices.size() > 3 && !CDTdone){
				mesh.loadEdgeFromMouse(clickPoint);
				clickPoint.clear();
				vertices.clear();
				normals.clear();
				colors.clear();
				mesh.runCDT();

				for (int i = 0; i < mesh.pNum; i++) {
					vertices.push_back(vec3(mesh.vertexBuffer[i*9], mesh.vertexBuffer[i*9+1], mesh.vertexBuffer[i*9+2]));
					colors.push_back(vec3(mesh.vertexBuffer[i*9+3], mesh.vertexBuffer[i*9+4], mesh.vertexBuffer[i*9+5]));
					normals.push_back(vec3(mesh.vertexBuffer[i*9+6], mesh.vertexBuffer[i*9+7], mesh.vertexBuffer[i*9+8]));
				}
				// Update the whole buffer and load it into a VBO
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
				glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
				glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
				CDTdone = true;
			}
		}
		if (glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS){
			mousePressR = true;
		}
		if (glfwGetMouseButton( GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_RELEASE){
			if(mousePressR){
				mousePressR = false;
			}
		}
		if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS){
			dy += 1.f;
		}
		if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS){
			dy -= 1.f;
		}
		if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS){
			dx += 1.f;
		}
		if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS){
			dx -= 1.f;
		}
		if (glfwGetKey( GLFW_KEY_F1 ) == GLFW_PRESS){
			glUseProgram(programID);
		}
		if (glfwGetKey( GLFW_KEY_F2 ) == GLFW_PRESS){
			glUseProgram(programID2);
		}
		if (glfwGetKey( GLFW_KEY_F3 ) == GLFW_PRESS){
			glUseProgram(programID3);
		}
		if (glfwGetKey( GLFW_KEY_F4 ) == GLFW_PRESS)
			pressF4 = true;
		if (glfwGetKey( GLFW_KEY_F4 ) == GLFW_RELEASE){	// prune + spine
			if(CDTdone && pressF4 && !pruneDone){
				vertices.clear();
				normals.clear();
				colors.clear();
				//mesh.loadTriangleFromPointSet(*mesh.ps);
				mesh.loadEdgeFromPointSet(*mesh.ps);

				for (int i = 0; i < mesh.pNum; i++) {
					vertices.push_back(vec3(mesh.vertexBuffer[i*9], mesh.vertexBuffer[i*9+1], mesh.vertexBuffer[i*9+2]));
					colors.push_back(vec3(mesh.vertexBuffer[i*9+3], mesh.vertexBuffer[i*9+4], mesh.vertexBuffer[i*9+5]));
					normals.push_back(vec3(mesh.vertexBuffer[i*9+6], mesh.vertexBuffer[i*9+7], mesh.vertexBuffer[i*9+8]));
				}
				// Update the whole buffer and load it into a VBO
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
				glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
				glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), &normals[0], GL_STATIC_DRAW);
				printf("pruning & spine done!\n");
				pruneDone = true;
			}
			if(pressF4)
				pressF4 = false;
		}
		if (glfwGetKey( GLFW_KEY_F5 ) == GLFW_PRESS){
			pressF5 = true;
		}
		if (glfwGetKey( GLFW_KEY_F5 ) == GLFW_RELEASE){
			if(pressF5){
				light1 = (light1 == vec3(0, 0, 0)) ? vec3(10000, 10000, 10000) : vec3(0, 0, 0);
				pressF5 = false;
			}
		}
		if (glfwGetKey( GLFW_KEY_F6 ) == GLFW_PRESS){
			pressF6 = true;
		}
		if (glfwGetKey( GLFW_KEY_F6 ) == GLFW_RELEASE){
			if(pressF6){
				light2 = (light2 == vec3(0, 0, 0)) ? vec3(10000, 10000, 10000) : vec3(0, 0, 0);
				pressF6 = false;
			}
		}
		if (glfwGetKey( GLFW_KEY_F7 ) == GLFW_PRESS){
			pressF7 = true;
		}
		if (glfwGetKey( GLFW_KEY_F7 ) == GLFW_RELEASE){
			if(pressF7){
				light3 = (light3 == vec3(0, 0, 0)) ? vec3(10000, 10000, 10000) : vec3(0, 0, 0);
				pressF7 = false;
			}
		}
		if (glfwGetKey( GLFW_KEY_F8 ) == GLFW_PRESS){
			pressF8 = true;
		}
		if (glfwGetKey( GLFW_KEY_F8 ) == GLFW_RELEASE){	// show spine
			if(CDTdone && pressF8 && pruneDone){
				
			}
			if(pressF8)
				pressF8 = false;
		}
		// Projection matrix
		mat4 ProjectionMatrix;
		if (glfwGetKey( GLFW_KEY_F9 ) == GLFW_PRESS || CDTdone)
			ProjectionMatrix = perspective(90.0f, 4.0f / 3.0f, 300.f, 1000.f);	// 90¢X Field of View, 4:3 ratio, display range : 300 unit <-> 1000 units
		else
			ProjectionMatrix = ortho(-512.f, 512.f, -350.f, 350.f, 300.f, 1000.f);
		// Camera matrix
		mat4 ViewMatrix = lookAt(vec3(0, 0, 512), 
			vec3(0, 0, 0),
			vec3(0, 1, 0));
		mat4 RotationMatrix = gtx::euler_angles::eulerAngleYXZ(gEulerRotation.y, gEulerRotation.x, gEulerRotation.z);
		if (glfwGetKey( GLFW_KEY_LCTRL ) == GLFW_PRESS)
			RotationMatrix = gtc::quaternion::mat4_cast(gQuaternion);
		mat4 TranslationMatrix = gtc::matrix_transform::translate(mat4(), vec3(dx, dy, -50 * glfwGetMouseWheel()));
		mat4 ScalingMatrix = gtc::matrix_transform::scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
		mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
		mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
		
		// Set up light
		vec3 lightPos = vec3(512, 350, 512);
		glUniform3f(LightID, lightPos.x + light1.x, lightPos.y + light1.y, lightPos.z + light1.z);
		lightPos = vec3(-512, -350, 512);
		glUniform3f(LightID2, lightPos.x + light2.x, lightPos.y + light2.y, lightPos.z + light2.z);
		lightPos = vec3(-512, -350, -512);
		glUniform3f(LightID3, lightPos.x + light3.x, lightPos.y + light3.y, lightPos.z + light3.z);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			vertexColorID,				  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);
			
		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,    // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_TRUE,                      // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);

		// Draw the triangles !
		if(!CDTdone || pressF8)
			glDrawArrays(GL_LINES, 0, vertices.size());
		else
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Draw GUI
		TwDraw();

		// Swap buffers
		glfwSwapBuffers();
		//gEulerRotation.y += 0.01;
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
