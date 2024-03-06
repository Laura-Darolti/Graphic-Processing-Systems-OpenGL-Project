#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 lightPos;  // Punctiform light position
uniform bool isLightActive;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//components
vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
//fog 
uniform bool isFogActive;
uniform float fogDensity=0.1f; // density of the fog
uniform vec3 fogColor=vec3(0.6f, 0.6f, 0.61f);    // color of the fog
//shadow
uniform sampler2D shadowMap; 
uniform mat4 lightSpaceMatrix; 

float ShadowCalculation(vec4 fragPosLightSpace) {
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    float shadow = projCoords.z > closestDepth ? 1.0 : 0.0;
    return shadow;
}
void computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor;

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}
void computePunctiformLight()
{
    // Compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    
    vec3 lightDirN = normalize(lightPos - fPosEye.xyz);

   
    vec3 viewDir = normalize(-fPosEye.xyz);

    // Compute ambient light
    ambient = ambientStrength * lightColor;

    // Compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;

    // Compute half vector (Blinn-Phong model)
    vec3 halfVector = normalize(lightDirN + viewDir);

    // Compute specular light (Blinn-Phong model)
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor;
}
void computeFog(vec3 color){
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    float distance = length(fPosEye.xyz);
    float fogFactor = 1.0 - exp(-pow((distance * fogDensity), 2)); 
    fogFactor = clamp(fogFactor, 0.0, 1.0); // Make sure it's in the range [0, 1]
    vec3 finalColor = mix(fogColor, color, fogFactor);
    fColor = vec4(finalColor, 1.0f);
}


void main() 
{
    // Reset lighting components
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);

     vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fPosition, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    
    if(shadow == 1.0) {
        diffuse *= 0.3; 
        specular = vec3(0.0); 
    }
    if (isLightActive) {
        
        computePunctiformLight();
    } else {
        
        computeDirLight();
    }


    // Compute final vertex color
    vec3 textureColor = texture(diffuseTexture, fTexCoords).rgb;
    vec3 color = min((ambient + diffuse) * textureColor + specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    
    // Activate fog
    if (isFogActive) {
        computeFog(color);
    } else {
        fColor = vec4(color, 1.0f);
    }
   
}

