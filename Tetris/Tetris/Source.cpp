#include <sstream>
#include <string>
#include <time.h>

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <SOIL.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "logicV2.hpp"
#include "Shader.hpp"

#define PI 3.141529

Field *field;

bool keys[1024]; //values for horizontal movement, vertical rotations, soft/hard drops
GLuint GAME_SPEED = 20;
const GLuint WIDTH = 300, HEIGHT = 600;
const char* res_dir = "..\\Tetris\\";

void key_callback(GLFWwindow*, int, int, int, int);
void do_movement();

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Tetris", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK)
		throw std::exception("GLEW failed ot intialize.");

	glViewport(0, 0, WIDTH, HEIGHT);

	//square 
	GLfloat vertices[] = {
		-1.0f, 1.0f, 0,		0.5f, 0.5f, 1.0f,	0, 1,	-.75, .75 ,0,	//every block will be a composite of two triangles
		-1.0f, -1.0f, 0,	0.2f, 1.0f, 0.11f,	0, 0,	-.75, -.75 , 0, //bare in mind, second set is random vals used purely for interpolation color effects
		1.0f, -1.0f, 0,		1.0f, 0.5f, 0.24f,	1, 0,	.75, -.75 , 0,
		1.0f, 1.0f, 0,		0.33f, 0.33f, 0.5f,	1, 1,	.75, .75 , 0
	};
	GLuint indices[] = {
		0, 1, 2, //first
		0, 2, 3  //second
	};

	//scale model 
	for (int i = 0; i < 40; i += 11) //MAGIC NUMBER ALERT!!!!!!!!!!! TIGHT LOOP BOUNDS TO PREVENT STACK CORRUPTION
	{
		vertices[i] *= 1.0f / GRID_X;
		vertices[i + 1] *= 1.0f / GRID_Y;
	}

	//Vertices, Indices
	GLuint VEBO[2], VAO;
	glGenBuffers(2, VEBO);
	glGenVertexArrays(1, &VAO);
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VEBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VEBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), NULL);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	//Textures
	GLuint background_texture;
	int width, height;
	unsigned char* image;
	if ((image = SOIL_load_image("..\\Tetris\\dankAF.png", &width, &height, 0, SOIL_LOAD_RGB)) == NULL) 
		throw std::exception("Image not found");
	glGenTextures(1, &background_texture);
	glBindTexture(GL_TEXTURE_2D, background_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	//INITIALIZE SHADER
	Shader* shader = new Shader("..\\Tetris\\vShader.glsl", "..\\Tetris\\fShader.glsl");
	Shader* bgShader = new Shader("..\\Tetris\\bgvShader.glsl", "..\\Tetris\\bgfShader.glsl");

	//INITIALIZE GAME VARIABLES
	srand(time(NULL));
	field = new Field();
	field->redefined_block_geometry();
	int loop_counter = 0;

	//glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window) && !field->GAME_OVER)
	{
		glfwPollEvents();
		//do_movement();

		glClearColor(0.5f, sin(fmod(glfwGetTime(), 2 * PI)), 0.5f, 1.0f);
		//glClearTexImage(background_texture, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

		
		bgShader->Use();
		glBindTexture(GL_TEXTURE_2D, background_texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		shader->Use();
		if (!(loop_counter++ % GAME_SPEED))
		{
			if (field->check_bassline())
				field->block_down();
			else
			{
				field->regen_grid();
				field->check_lines();
				field->redefined_block_geometry();
			}
		}

		for (int i = 0; i < GRID_Y; i++)
		{
			for (int j = 0; j < GRID_X; j++)
			{
				if (field->grid[i][j])
				{
					//GRID IS ROW (Y-COORD) MAJOR --- SWITCH COORDINATES!!
					glUniform2f(glGetUniformLocation(shader->Program, "pos"), (GLfloat) j/ GRID_X, (2 * (GLfloat) i) / GRID_Y);
					glUniform1f(glGetUniformLocation(shader->Program, "time"), (GLfloat)glfwGetTime());
					glUniform1f(glGetUniformLocation(shader->Program, "blockColor"), (GLfloat) field->grid[i][j] / 10);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}
			}
		}
		

		//glUniform2f(glGetUniformLocation(shader->Program, "pos"), 0, 1.0f / 20.0f);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		for (GLfloat i = 0; i < 7; i += 2)
		{
			glUniform2f(glGetUniformLocation(shader->Program, "pos"), (GLfloat) field->cur_block_geometry[i] / GRID_X, 
				(2 * (GLfloat)field->cur_block_geometry[i + 1]) / GRID_Y);
			glUniform1f(glGetUniformLocation(shader->Program, "blockColor"), 0);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, VEBO);

	glfwTerminate();
	return 0;
}

 //Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		field->block_horizontal(0);
	if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT))
		field->block_horizontal(1);
	if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT))
		field->block_rotate(0);
	if (key == GLFW_KEY_X && (action == GLFW_PRESS || action == GLFW_REPEAT))
		field->block_rotate(1);
	if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT))
		GAME_SPEED = 4;
	else
		GAME_SPEED = 20;
}

//void do_movement()
//{
//	if (keys[GLFW_KEY_LEFT])
//		field->block_horizontal(0);
//	if (keys[GLFW_KEY_RIGHT])
//		field->block_horizontal(1);
//	if (keys[GLFW_KEY_DOWN])
//		GAME_SPEED = 4;
//	else
//		GAME_SPEED = 20;
//}