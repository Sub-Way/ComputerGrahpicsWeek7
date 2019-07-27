#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "OBJRead.h"
#include "STLRead.h"

const int width_window = 1000;
const int height_window = 1000;

int main(void)
{
	GLFWwindow *window = nullptr;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width_window, height_window, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	// callbacks here

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glClearColor(1, 1, 1, 1); // while background

	printf("%s\n", glGetString(GL_VERSION));

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	glOrtho(-1.2, 1.2, -1.2, 1.2, -100.0, 100.0);
	gluLookAt(1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 1, 0);

	//read 3D model from obj file
	OBJRead obj_reader;
	obj_reader.readOBJ("./earth.obj");
	//obj_reader.readObj("../../test-result-sphere_f80.obj");
	//obj_reader.dump();
	
	STLRead stl_reader;
	stl_reader.readSTL("./Cat.stl");
	//stl_reader.dump();
	
	// TODO: copy geometry to buffer object
	GLuint o_vbo[3];
	glGenBuffers(3, o_vbo);

	GLuint s_vbo[3];
	glGenBuffers(3, s_vbo);


	// glm::vec3 == vertex_stack_  (float, float, float) 
	//stl
	glBindBuffer(GL_ARRAY_BUFFER, o_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, obj_reader.vertex_stack_.size() * sizeof(float) * 3,
		&obj_reader.vertex_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, o_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, obj_reader.vertex_stack_.size() * sizeof(float) * 3, &obj_reader.vertex_stack_[0], GL_STATIC_DRAW);

	// glm::tvec3<unsigned int> (unsigned int, unsigned int, unsigned int)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o_vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj_reader.index_stack_.size() * sizeof(unsigned int) * 3 , &obj_reader.index_stack_[0], GL_STATIC_DRAW);

	//obj
	glBindBuffer(GL_ARRAY_BUFFER, s_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, stl_reader.vertex_stack_.size() * sizeof(float) * 3,
		&stl_reader.vertex_stack_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, stl_reader.vertex_stack_.size() * sizeof(float) * 3, &stl_reader.vertex_stack_[0], GL_STATIC_DRAW);

	// glm::tvec3<unsigned int> (unsigned int, unsigned int, unsigned int)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_vbo[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, stl_reader.index_stack_.size() * sizeof(unsigned int) * 3, &stl_reader.index_stack_[0], GL_STATIC_DRAW);


	// depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // <- depth test
		
		glTranslatef(-1, 0, 0);
		glRotatef(1, 0, 1, 0);
		glBindBuffer(GL_ARRAY_BUFFER, o_vbo[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ARRAY_BUFFER, o_vbo[1]);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o_vbo[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, stl_reader.index_stack_.size() * sizeof(unsigned int) * 3, GL_UNSIGNED_INT, 0);
		glEnd();

		glTranslatef(1, 0, 0);
		glRotatef(1, 0, 1, 0);
		glBindBuffer(GL_ARRAY_BUFFER, s_vbo[0]);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ARRAY_BUFFER, s_vbo[1]);
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3, GL_FLOAT, 0, 0);		// Vector 3

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_vbo[2]);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, stl_reader.index_stack_.size() * sizeof(unsigned int) * 3, GL_UNSIGNED_INT, 0);
		glEnd();

		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	glDeleteBuffersARB(3, o_vbo);
	glDeleteBuffersARB(3, s_vbo);

	glfwTerminate();

	return 0;
}

