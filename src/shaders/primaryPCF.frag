#version 330 core

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

in vec4 v2f_position;  // position
in vec4 v2f_positionL;  // position in light space
in vec4 v2f_normal;    // normal
in vec4 v2f_color;
in Material v2f_material;

uniform bool useLighting;
uniform vec4 viewPos;
uniform vec4 lightPosition;  // light position in camera space
uniform vec4 lightColor;
uniform sampler2D shadowDepthTex;
uniform int PCFSamples;

out vec4 out_color;

float shadowBias = 0.005;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowDepthTex, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowDepthTex, 0);
    for(int x = -PCFSamples; x <= PCFSamples; ++x) {
        for(int y = -PCFSamples; y <= PCFSamples; ++y) {
            float pcfDepth = texture(shadowDepthTex, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - shadowBias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= pow(PCFSamples * 2 + 1, 2);
    //shadow = currentDepth - shadowBias > closestDepth ? 1.0 : 0.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main() {
    if(useLighting) {
        // Compute the diffuse component
        vec4 N = normalize(v2f_normal);
        vec4 L = normalize(lightPosition - v2f_position);
        vec4 Diffuse =  max(dot(L, N), 0) * lightColor * v2f_material.diffuse;
         
        // Compute the specular component
        vec4 V = normalize(viewPos - v2f_position);
        vec4 H = normalize(L + V);
        vec4 Specular = pow(max(dot(H, N), 0), v2f_material.shininess) * lightColor * v2f_material.specular;
        
        // combine light components
        float shadow = ShadowCalculation(v2f_positionL);
        vec4 lighting = max((Diffuse + Specular) * (1.0 - shadow), v2f_material.ambient);
        out_color = lighting;
    }
    else {
        //out_color = v2f_color;
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
} 