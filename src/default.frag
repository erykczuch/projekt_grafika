#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 Normal;
in vec3 crntPos;

uniform sampler2D tex0;

uniform vec4 lightColor;
uniform vec3 lightPos;

uniform vec3 camPos;

uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.6;
uniform float shininess = 32.0;

void main()
{
    vec3 texColor = texture(tex0, texCoord).rgb;

    vec3 ambient = ambientStrength * lightColor.rgb;
    
    vec3 N = normalize(Normal);
    vec3 L = normalize(lightPos - crntPos);
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * lightColor.rgb;

    vec3 V = normalize(camPos - crntPos);
    vec3 H = normalize(L + V); // halfway vector

    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor.rgb;

    vec3 result = (ambient + diffuse + specular) * texColor;
    FragColor = vec4(result, 1.0);
}