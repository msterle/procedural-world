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
uniform int shadowSamples;

out vec4 out_color;

vec4 positionLDivW;

float chebyshev(float distance) {
    vec3 projCoords = v2f_positionL.xyz / v2f_positionL.w;
    vec2 moments = texture2D(shadowDepthTex, projCoords.xy).rg;

    

    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002);
    float d = distance - moments.x;
    float p_max = variance / (variance + d * d);

    return p_max;
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
        positionLDivW = v2f_positionL / v2f_positionL.w;
        float shadow = chebyshev(positionLDivW.z);
        vec4 lighting = max((Diffuse + Specular) * shadow, v2f_material.ambient);
        out_color = lighting;
    }
    else {
        //out_color = v2f_color;
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
} 