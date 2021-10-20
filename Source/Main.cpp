/* OpenGL resources: https://learnopengl.com/
	Inspiration for implementation by KleberPF: https://www.reddit.com/r/opengl/comments/p3z86w/sierpinski_triangle_from_dice_rolls_simulation/
	Set up (OpenGL tutorial by Victor Gordan): https://www.youtube.com/watch?v=XpBGwZNyUh0
	Chaos Game by Numberphile: https://www.youtube.com/watch?v=kbKtFN71Lfs&t
	Chaos Game wikipedia: https://en.wikipedia.org/wiki/Chaos_game
	More Chaos Game resources by Mitch Richling: https://www.mitchr.me/SS/ChaosGame/index.html
*/
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <ctime>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <math.h>

#include "shaderClass.h"

using namespace std;

unsigned __int64 rand_num_gen(int, int);
void chaos_fractal_builder(int, int, vector<glm::vec3>&);

#define PI 3.14159265
constexpr int MAX_POINT_NUM = 90000;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1080, 1080, "OpenglWindow", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create the opengl window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 1080, 1080);

	// build and compile our shader program
	Shader ourShader("default.vert", "default.frag");

	vector<glm::vec3>vertices;
	/* Control Guide 
	chaos_fractal_builder(3, 1, vertices); > Triangle: Sierpinski Triangle, No Restriction
	chaos_fractal_builder(4, 1, vertices); > Rectangle: The current vertex cannot be the previous vertex
	chaos_fractal_builder(4, 2, vertices); > Rectangle: The current vertex cannot be one place away from the previous vertex (anti-clockwise)
	chaos_fractal_builder(4, 3, vertices); > Rectangle: The current vertex cannot be one place away from the previous vertex (clockwise)
	chaos_fractal_builder(4, 4, vertices); > Rectangle: The current vertex cannot be two places away from the previous vertex (anti-clockwise)
	chaos_fractal_builder(4, 5, vertices); > Rectangle: The current vertex cannot be neighbor the previously chosen vertex if the two previously chosen vertices are the same
	chaos_fractal_builder(5, 1, vertices); > Pentagon: No Restriction
	chaos_fractal_builder(5, 2, vertices); > Pentagon: The current vertex cannot be the previous vertex
	chaos_fractal_builder(5, 3, vertices); > Pentagon: The current vertex cannot be one place away (anti-clockwise)
	chaos_fractal_builder(5, 4, vertices); > Pentagon: The current vertex cannot be neighbor the previously chosen vertex if the two previously chosen vertices are the same
	*/
	chaos_fractal_builder(3, 1, vertices);

	GLuint VBOs[1];
	GLuint VAOs[1];
	glGenVertexArrays(1, VAOs);
	glGenBuffers(1, VBOs);
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAOs[0]);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/* specify the color of the background */
	glClearColor(0.01f, 0.01f, 0.05f, 1.0f);
	glPointSize(1.0);

	ourShader.use();
	glBindVertexArray(VAOs[0]);
	
	// for slowly showing each vertex one by one.
	int pointCount = -100;
	float interval = 0.001;
	float elapsed_time_updates_with_interval = 0;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		float timeValue = glfwGetTime();
		ourShader.setFloat("time", timeValue);
		float offset = 0.0f;
		ourShader.setFloat("xOffset", offset);

		if (timeValue > elapsed_time_updates_with_interval && MAX_POINT_NUM > pointCount) {
			elapsed_time_updates_with_interval += interval;
			if (pointCount < 1600) {
				pointCount += 1;
			} else if (pointCount < 2660) {
				pointCount += 2;
			} else if (pointCount < 4780) {
				pointCount += 4;
			} else if (pointCount < 9020) {
				pointCount += 8;
			} else if (pointCount < 17500) {
				pointCount += 16;
			} else if (pointCount < 34460) {
				pointCount += 32;
			} else {
				pointCount += 96;
			}
		} else {
			cout << "Done!" << endl;
		}

		glDrawArrays(GL_POINTS, 0, pointCount + 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

unsigned __int64 rand_num_gen(int min, int max) {
	// inclusive min and max
	return (unsigned __int64) (rand() % ((max - min) + 1)) + min;
}

void chaos_fractal_builder(int number_of_points, int restriction, vector<glm::vec3> &vertices) {
	// Triangle
	if (number_of_points == 3) {
			// Sierpinski Triangle
			vertices = {
				// positions					// colors
				glm::vec3(-0.6f, -0.5f, 0.0f),  glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(0.6f,  -0.5f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(0.0f,   0.5f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f)
			};
			// starting point
			glm::vec3 currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 currentColor = glm::vec3(0.5f, 0.5f, 0.5f);
			vertices.push_back(currentPosition);
			vertices.push_back(currentColor);

			// Fractal generation
			srand(time(NULL));
			for (int i = 1; i < MAX_POINT_NUM; i++) {
				int dice_roll = rand_num_gen(0, 2);
				glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
				glm::vec3 newColor = glm::vec3(newPosition);
				vertices.push_back(newPosition);
				vertices.push_back(newColor);
				currentPosition = newPosition;
			}
		}
		// Rectangle: chaos game with the restrictions
	else if (number_of_points == 4) {
			vertices = {
				// positions					// colors
				glm::vec3(0.7f, 0.7f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
				glm::vec3(-0.7f, 0.7f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec3(-0.7f, -0.7f, 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f),
				glm::vec3(0.7f, -0.7f, 0.0f),   glm::vec3(0.0f, 0.0f, 1.0f)
			};

			// starting point
			glm::vec3 currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 currentColor = glm::vec3(0.5f, 0.5f, 0.5f);
			vertices.push_back(currentPosition);
			vertices.push_back(currentColor);

			int prev_roll = 1;
			int dice_roll = 1;
			// Fractal generations
			if (restriction == 1) {
				// 1. Restriction: The current vertex cannot be the previous vertex
				srand(time(NULL));
				for (int i = 1; i < MAX_POINT_NUM; i++) {
					while (prev_roll == dice_roll) {
						dice_roll = rand_num_gen(0, 3);
					}
					prev_roll = dice_roll;
					glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
					glm::vec3 newColor = glm::vec3(newPosition);
					vertices.push_back(newPosition);
					vertices.push_back(newColor);
					currentPosition = newPosition;
				}
			}
			else if (restriction == 2) {
				// 2.0 Restriction: The current vertex cannot be one place away from the previous vertex (anti-clockwise)
				srand(time(NULL));
				for (int i = 1; i < MAX_POINT_NUM; i++) {
					// Based on a previous roll, we update the prev_roll to the vertex we want to avoid.
					// anti-clockwise, cannot be one place away
					if (prev_roll == 0) {
						prev_roll = 3;
					}
					else {
						prev_roll -= 1;
					}
					// keep rolling until the vertex is the one we don't have to avoid.
					dice_roll = rand_num_gen(0, 3);
					while (prev_roll == dice_roll) {
						dice_roll = rand_num_gen(0, 3);
					}
					glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
					prev_roll = dice_roll;
					glm::vec3 newColor = glm::vec3(newPosition);
					vertices.push_back(newPosition);
					vertices.push_back(newColor);
					currentPosition = newPosition;
				}
			}
			else if (restriction == 3) {
				// 2.1 Restriction: The current vertex cannot be one place away from the previous vertex (clockwise)
				srand(time(NULL));
				for (int i = 1; i < MAX_POINT_NUM; i++) {
					// Based on a previous roll, we update the prev_roll to the vertex we want to avoid.
					// clockwise, cannot be one place away
					if (prev_roll == 3) {
						prev_roll = 0;
					}
					else {
						prev_roll += 1;
					}
					// keep rolling until the vertex is the one we don't have to avoid.
					dice_roll = rand_num_gen(0, 3);
					while (prev_roll == dice_roll) {
						dice_roll = rand_num_gen(0, 3);
					}
					glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
					prev_roll = dice_roll;
					glm::vec3 newColor = glm::vec3(newPosition);
					vertices.push_back(newPosition);
					vertices.push_back(newColor);
					currentPosition = newPosition;
				}
			}
			else if (restriction == 4) {
				// 2.2 Restriction: The current vertex cannot be two places away from the previous vertex (anti-clockwise)
				srand(time(NULL));
				for (int i = 1; i < MAX_POINT_NUM; i++) {
					// Based on a previous roll, we update the prev_roll to the vertex we want to avoid.
					// anti-clockwise, cannot be two places away
					prev_roll = (prev_roll + 2) % 4;
					// keep rolling until the vertex is the one we don't have to avoid.
					dice_roll = rand_num_gen(0, 3);
					while (prev_roll == dice_roll) {
						dice_roll = rand_num_gen(0, 3);
					}
					glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
					prev_roll = dice_roll;
					glm::vec3 newColor = glm::vec3(newPosition);
					vertices.push_back(newPosition);
					vertices.push_back(newColor);
					currentPosition = newPosition;
				}
			}
			else if (restriction == 5) {
				// 3. Restriction: The current vertex cannot be neighbor the previously chosen vertex
				//	if the two previously chosen vertices are the same.
				prev_roll = 1; // keep track of the previous roll
				int prev_of_prev_roll = 1; // keep track of older previous roll
				int avoid_num_clock = 1; // clockwise neighbor to avoid
				int avoid_num_anti = 1; // anti-clockwise neighbor to avoid
				srand(time(NULL));
				for (int i = 1; i < MAX_POINT_NUM; i++) {
					// if the two previously chosen vertices are the same.
					if (prev_roll == prev_of_prev_roll) {
						// designate previously chosen vertex's neighbor
						avoid_num_clock = (prev_roll + 3) % 4;
						avoid_num_anti = (prev_roll + 1) % 4;
						// keep rolling until the vertex is the one we don't have to avoid.
						dice_roll = rand_num_gen(0, 3);
						while (dice_roll == avoid_num_clock || dice_roll == avoid_num_anti) {
							dice_roll = rand_num_gen(0, 3);
						}
					}
					else {
						// otherwise, just a standard roll
						dice_roll = rand_num_gen(0, 3);
					}
					// recording roll histories up to 2 previous rolls.
					prev_of_prev_roll = prev_roll;
					prev_roll = dice_roll;
					glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
					glm::vec3 newColor = glm::vec3(newPosition);
					vertices.push_back(newPosition);
					vertices.push_back(newColor);
					currentPosition = newPosition;
				}
			}
		}
	else if (number_of_points == 5) {
		vertices = {
			// positions					// colors
			glm::vec3(0.0, 0.8, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.8*cos(18 * PI / 180.0), 0.8*sin(18 * PI / 180.0), 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.8*cos(-54 * PI / 180.0), 0.8*sin(-54 * PI / 180.0), 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(-0.8 * cos(54 * PI / 180.0), 0.8 * sin(-54 * PI / 180.0), 0.0f),  glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(-0.8 * cos(18 * PI / 180.0), 0.8 * sin(18 * PI / 180.0), 0.0f),   glm::vec3(0.0f, 0.0f, 1.0f)
		};

		// starting point
		glm::vec3 currentPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 currentColor = glm::vec3(0.5f, 0.5f, 0.5f);
		vertices.push_back(currentPosition);
		vertices.push_back(currentColor);

		int prev_roll = 1;
		int dice_roll = 1;
		if (restriction == 1) {
			// 1. no restriction
			srand(time(NULL));
			for (int i = 1; i < MAX_POINT_NUM; i++) {
				dice_roll = rand_num_gen(0, 4);
				glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
				glm::vec3 newColor = glm::vec3(newPosition);
				vertices.push_back(newPosition);
				vertices.push_back(newColor);
				currentPosition = newPosition;
			}
		} else if (restriction == 2) {
			// 2. Restriction: The current vertex cannot be the previous vertex
			srand(time(NULL));
			for (int i = 1; i < MAX_POINT_NUM; i++) {
				while (prev_roll == dice_roll) {
					dice_roll = rand_num_gen(0, 4);
				}
				prev_roll = dice_roll;
				glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
				glm::vec3 newColor = glm::vec3(newPosition);
				vertices.push_back(newPosition);
				vertices.push_back(newColor);
				currentPosition = newPosition;
			}
		} else if (restriction == 3) {
			// 3. Restriction: The current vertex cannot be one place away (anti-clockwise)
			srand(time(NULL));
			for (int i = 1; i < MAX_POINT_NUM; i++) {
				// Based on a previous roll, we update the prev_roll to the vertex we want to avoid.
				// anti-clockwise, cannot be one place away
				prev_roll = (prev_roll + 4) % 5;
				// keep rolling until the vertex is the one we don't have to avoid.
				dice_roll = rand_num_gen(0, 4);
				while (prev_roll == dice_roll) {
					dice_roll = rand_num_gen(0, 4);
				}
				glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
				prev_roll = dice_roll;
				glm::vec3 newColor = glm::vec3(newPosition);
				vertices.push_back(newPosition);
				vertices.push_back(newColor);
				currentPosition = newPosition;
			}
		} else if (restriction == 4) {
			// 4. Restriction: The current vertex cannot be neighbor the previously chosen vertex
			//	if the two previously chosen vertices are the same
			prev_roll = 1; // keep track of the previous roll
			int prev_of_prev_roll = 1; // keep track of older previous roll
			int avoid_num_clock = 1; // clockwise neighbor to avoid
			int avoid_num_anti = 1; // anti-clockwise neighbor to avoid
			srand(time(NULL));
			for (int i = 1; i < MAX_POINT_NUM; i++) {
				// if the two previously chosen vertices are the same.
				if (prev_roll == prev_of_prev_roll) {
					// designate previously chosen vertex's neighbor
					avoid_num_clock = (prev_roll + 4) % 5;
					avoid_num_anti = (prev_roll + 1) % 5;
					// keep rolling until the vertex is the one we don't have to avoid.
					dice_roll = rand_num_gen(0, 4);
					while (dice_roll == avoid_num_clock || dice_roll == avoid_num_anti) {
						dice_roll = rand_num_gen(0, 4);
					}
				}
				else {
					// otherwise, just a standard roll
					dice_roll = rand_num_gen(0, 4);
				}
				// recording roll histories up to 2 previous rolls.
				prev_of_prev_roll = prev_roll;
				prev_roll = dice_roll;
				glm::vec3 newPosition = (currentPosition + vertices[dice_roll * (unsigned __int64)2]) / 2.0f;
				glm::vec3 newColor = glm::vec3(newPosition);
				vertices.push_back(newPosition);
				vertices.push_back(newColor);
				currentPosition = newPosition;
			}
		}
	}
}
