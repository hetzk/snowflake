#include "ros.h"

using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::vector;

Ros::Ros() {
	
}

Ros::~Ros() {
}

// Initialize the graphics etc.
void Ros::Init() {
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

	GLenum glew_result = glewInit();

	if(glew_result != GLEW_OK) {
		cout << "Error initializing GLEW" << endl;
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_result));
	}

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Camera
	camera_position_ = glm::vec3(4.0f, 3.0f, 3.0f);
	theta_ = 0.0f;
	phi_ = 0.0f;

	// FPS
	fps_last_time_ = clock_.GetTime();
	fps_ = 0;
	fps_frames_since_ = 0;
}

// Render the content.
void Ros::Render() {
	// Clear and reset
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Calculate FPS
	fps_current_time_ = clock_.GetTime();

	if(fps_current_time_ - fps_last_time_ >= 1000) {
		fps_last_time_ = fps_current_time_;
		fps_ = fps_frames_since_;
		fps_frames_since_ = 0;
	}

	++fps_frames_since_;

	// Update model etc. and render.
	mvp_ = projection_ * view_ * model_;

	glUseProgram(shader_program_);

	glUniformMatrix4fv(matrix_, 1, GL_FALSE, &mvp_[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
	glDrawElements(GL_TRIANGLES, vertices_, GL_UNSIGNED_INT, (void*)0);
	
	glDisableVertexAttribArray(0);

	clock_.Slice();
}

// Print FPS.
void Ros::PrintFPS() {
	cout << "FPS = " << fps_ << endl;
}

// Process keyboard input.
void Ros::ProcessInput(const int key) {
	switch(key) {
		case keys::Key_Return:
			PrintFPS();
			break;
		case keys::Key_1:
			if(glIsEnabled(GL_CULL_FACE))
				glDisable(GL_CULL_FACE);
			else glEnable(GL_CULL_FACE);
			break;
		case keys::Key_2:
			ToggleWireframe();
	}
}

// Toggle wireframe.
void Ros::ToggleWireframe() {
	GLint polygon_mode[2];
	glGetIntegerv(GL_POLYGON_MODE, polygon_mode);

	if(polygon_mode[1] == GL_LINE)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

// Load a mesh/model from a file.
void Ros::LoadModel(const char* path) {
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	vector<GLfloat> vertex_buffer_data;
	vector<unsigned int> index_buffer_data;
	vector<glm::vec3> vertex_buffer;
	unsigned int vertices = 0, normals = 0, faces = 0, joints = 0, animations = 0;

    ifstream fin(path, std::ios_base::in | std::ios_base::binary);
    if(fin.is_open()) {
    	// Mesh
		fin.read((char*)&vertices, sizeof(int));
		fin.read((char*)&normals, sizeof(int));
		fin.read((char*)&faces, sizeof(int));
		fin.read((char*)&joints, sizeof(int));
		fin.read((char*)&animations, sizeof(int));

		cout << "Read the following from " << path << ":" << endl;
		cout << "\t" << vertices << " vertices" << endl;
		cout << "\t" << normals << " normals" << endl;
		cout << "\t" << faces << " faces" << endl;

		// Mesh - vertices
		for(unsigned int i = 0; i < vertices; ++i) {
			glm::vec3 vertex;
			fin.read((char*)&vertex.x, sizeof(float));
			fin.read((char*)&vertex.y, sizeof(float));
			fin.read((char*)&vertex.z, sizeof(float));
			cout << "\tcoords(" << i << ") = " << vertex.x << ", " << vertex.y << ", " <<  vertex.z << endl;
			vertex_buffer.push_back(vertex);
		}

		// Mesh - normals
		for(unsigned int i = 0; i < normals; ++i) {
			glm::vec3 normal;
			fin.read((char*)&normal.x, sizeof(float));
			fin.read((char*)&normal.y, sizeof(float));
			fin.read((char*)&normal.z, sizeof(float));
		}

		// Faces
		for(unsigned int i = 0; i < faces; ++i) {	// For each face...
			for(unsigned int j = 0; j < 3; ++j) {	// ... read all three vertices
				unsigned int vertex_id = 0;
				unsigned int normal_id = 0;
				fin.read((char*)&vertex_id, sizeof(int));
				fin.read((char*)&normal_id, sizeof(int));

				index_buffer_data.push_back(vertex_id);
			}
		}

		fin.close();
	}

	// Vertices
	glGenBuffers(1, &vertex_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer.size()*sizeof(glm::vec3), &vertex_buffer[0], GL_STATIC_DRAW);
	vertices_ = faces * 3;	// Three vertices per face

	// Indices
	glGenBuffers(1, &index_buffer_);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(unsigned int), &index_buffer_data[0], GL_STATIC_DRAW);

	shader_program_ = LoadShaders();

	projection_ = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

	// Camera matrix
	view_ = glm::lookAt(
		glm::vec3(4.0f, 3.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
		);

	model_ = glm::mat4(1.0f);
	
	mvp_ = projection_ * view_ * model_;

	matrix_ = glGetUniformLocation(shader_program_, "mvp");
}

// Update camera position.
void Ros::UpdateCamera(int x, int y) {
	theta_ += x * 0.003f;
	phi_ += y * 0.003f;

	glm::vec3 pos = glm::vec3(2.0f*cos(theta_), 2.0f*phi_, 2.0f*sin(theta_));

	view_ = glm::lookAt(
		camera_position_ * pos,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
}

// Load shaders from files.
GLuint Ros::LoadShaders() {
	string vertex_shader_path = "model.vs";
	string fragment_shader_path = "model.fs";

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	// Read from file
	string vertex_shader_data;
	ifstream vertex_shader_stream(vertex_shader_path.c_str(), std::ios::in);
	if(vertex_shader_stream.is_open()) {
		string line = "";
		while(getline(vertex_shader_stream, line))
			vertex_shader_data += "\n" + line;
		vertex_shader_stream.close();
	}

	string fragment_shader_data;
	ifstream fragment_shader_stream(fragment_shader_path.c_str(), std::ios::in);
	if(fragment_shader_stream.is_open()) {
		string line = "";
		while(getline(fragment_shader_stream, line))
			fragment_shader_data += "\n" + line;
		fragment_shader_stream.close();
	}

	GLint result = GL_FALSE;
	int info_log_length;

	// Compile vertex shader
	cout << "Compiling shader " << vertex_shader_path << ".. ";
	char const* vertex_source_pointer = vertex_shader_data.c_str();
	glShaderSource(vertex_shader, 1, &vertex_source_pointer, NULL);
	glCompileShader(vertex_shader);

	// Check vertex shader
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &info_log_length);
	char vertex_shader_error[info_log_length];
	glGetShaderInfoLog(vertex_shader, info_log_length, NULL, vertex_shader_error);
	if(strlen(vertex_shader_error) > 0) cout << vertex_shader_error << endl;
	else cout << "OK" << endl;

	// Compile fragment shader
	cout << "Compiling shader " << fragment_shader_path << ".. ";
	char const* fragment_source_pointer = fragment_shader_data.c_str();
	glShaderSource(fragment_shader, 1, &fragment_source_pointer, NULL);
	glCompileShader(fragment_shader);

	// Check fragment shader
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &info_log_length);
	char fragment_shader_error[info_log_length];
	glGetShaderInfoLog(fragment_shader, info_log_length, NULL, fragment_shader_error);
	if(strlen(fragment_shader_error) > 0) cout << fragment_shader_error << endl;
	else cout << "OK" << endl;

	// Link program
	cout << "Linking program.. ";
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	// Check program
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
	char program_error[info_log_length];
	glGetProgramInfoLog(program, info_log_length, NULL, program_error);
	if(strlen(program_error) > 0) cout << program_error << endl;
	else cout << "OK" << endl;

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program;
}