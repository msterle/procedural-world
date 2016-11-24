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
in vec2 v2f_texcoords;
in Material v2f_material;

uniform bool useLighting;
uniform bool isTextured;
uniform bool isTerrain;
uniform vec4 viewPos;
uniform vec4 lightPosition;  // light position in camera space
uniform vec4 lightColor;
uniform sampler2D shadowDepthTex;
uniform sampler2D meshTex;
uniform sampler2D snowTex;
uniform sampler2D grassTex;
uniform sampler2D rockTex;
uniform int PCFSamples;

out vec4 out_color;

float chebyshev(vec3 projCoords) {
    vec2 moments = texture2D(shadowDepthTex, projCoords.xy).rg;

    if (projCoords.z <= moments.x)
        return 1.0;

    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.00002);
    float d = projCoords.z - moments.x;
    float p_max = variance / (variance + d * d);

    return p_max;
}

vec4 getDiffuseColor() {
    if(!isTerrain)
        return texture2D(meshTex, v2f_texcoords);
    float dx = dFdx(v2f_normal.y);
    float dy = dFdy(v2f_normal.y);
    //if(v2f_normal.x * v2f_normal.x + v2f_normal.z * v2f_normal.z > 0.25)
    if(dx * dx + dy * dy > 0.0001)
        return texture2D(rockTex, v2f_texcoords);
    if(v2f_position.y > 355)
        return texture2D(snowTex, v2f_texcoords) ;
    return texture2D(grassTex, v2f_texcoords);
    //float moment = dx*dx+dy*dy; // slope squared
    //texture2D(meshTex, v2f_texcoords)
}

void main() {
    vec4 diffuseColor = isTextured ? getDiffuseColor() : v2f_material.diffuse;
    vec4 ambientColor = isTextured ? diffuseColor / 8 : v2f_material.ambient;

    if(useLighting) {
        // Compute the diffuse component
        vec4 L = normalize(lightPosition - v2f_position);
        vec4 Diffuse =  max(dot(L, v2f_normal), 0) * lightColor * diffuseColor;
         
        // Compute the specular component
        vec4 V = normalize(viewPos - v2f_position);
        vec4 H = normalize(L + V);
        vec4 Specular = pow(max(dot(H, v2f_normal), 0), v2f_material.shininess) * lightColor * v2f_material.specular;
        
        // combine light components
        vec3 projCoords = v2f_positionL.xyz / v2f_positionL.w * 0.5 + 0.5;
        float shadow = chebyshev(projCoords);
        vec4 lighting = max((Diffuse + Specular) * shadow, ambientColor);
        out_color = lighting;
    }
    else {
        out_color = v2f_color;
    }
} 