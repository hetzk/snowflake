#version 330 core

// Input vertex data, different for all executions of this shader
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh
uniform mat4 mvp;

void main() {
	
	// Output position of the vertex, in clip space : mvp * position
	vec4 v = vec4(vertexPosition_modelspace, 1);	// Transform a homogenous 4D vector
	gl_Position = mvp * v;

	//gl_Position.xyz = vertexPosition_modelspace;
	//gl_Position.w = 1.0;
}