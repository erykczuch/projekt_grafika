#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors
layout (location = 1) in vec3 aColor;
// Texture Coordinates
layout (location = 2) in vec2 aTex;

// Outputs the color for the Fragment Shader
out vec3 color;
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

// Imports the camera matrix from the main function
uniform mat4 camMatrix;
// Imports the model matrix to position individual cubes
uniform mat4 model;

void main()
{
	// Multiply the camera matrix AND model matrix by the position
	gl_Position = camMatrix * model * vec4(aPos, 1.0);

	color = aColor;
	texCoord = aTex;
}