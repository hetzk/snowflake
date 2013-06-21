/*

	Ros, the engine.

*/

#ifndef SNOWFLAKE_ROS_H_
#define SNOWFLAKE_ROS_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstring>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "common.h"
#include "clock.h"

namespace keys {
	const int Key_Escape = XK_Escape;
	const int Key_Return = XK_Return;
	const int Key_1 = XK_1;
	const int Key_2 = XK_2;
	const int Key_t = XK_t;
}

class Ros {
public:
	Ros();
	~Ros();
	void Init();
	GLuint LoadShaders();
	void LoadModel(const char* path);
	void PrintFPS();
	void ProcessInput(const int key);
	void Render();
	void ToggleWireframe();
	void UpdateCamera(int x, int y);
private:
	Clock clock_;

	// Fps
	int fps_;
	int fps_last_time_;
	int fps_current_time_;
	int fps_frames_since_;

	// Model
	GLuint vertex_buffer_;
	GLuint index_buffer_;
	GLuint shader_program_;
	glm::mat4 mvp_;
	glm::mat4 view_;
	glm::mat4 model_;
	glm::mat4 projection_;
	unsigned int vertices_;

	GLuint matrix_;	

	// Camera
	glm::vec3 camera_position_;
	float theta_;
	float phi_;

	DISALLOW_COPY_AND_ASSIGN(Ros);
};

#endif	// SNOWFLAKE_ROS_H_