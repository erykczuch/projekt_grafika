#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Inputs the color from the Vertex Shader
in vec3 color;

// Inputs the texture coordinates from the Vertex Shader
in vec2 texCoord;

// Gets the Texture Unit from the main function
// IMPORTANT: This name "tex0" MUST match what you wrote in C++
uniform sampler2D tex0;

void main()
{
    // Simply output the texture color
    FragColor = texture(tex0, texCoord);
    
    // (Optional) If you want the vertex colors to mix with the texture, use this instead:
    // FragColor = texture(tex0, texCoord) * vec4(color, 1.0f);
}