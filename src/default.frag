#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;
in vec3 Normal;
in vec3 crntPos;
in vec4 FragPosLightSpace;

uniform sampler2D tex0;
uniform sampler2D shadowMap;

uniform vec4 lightColor;
uniform vec3 lightPos;

uniform vec3 camPos;

uniform float ambientStrength = 0.1;
uniform float specularStrength = 0.6;
uniform float shininess = 32.0;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
        
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    float bias = max(0.0005 * (1.0 - dot(normal, lightDir)), 0.00005);
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    return shadow;
}

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

    float shadow = ShadowCalculation(FragPosLightSpace, N, L);

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor;
    FragColor = vec4(result, 1.0);
}