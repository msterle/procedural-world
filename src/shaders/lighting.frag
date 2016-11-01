#version 330 core

in vec4 v2f_positionC;  // position in camera space
in vec4 v2f_normalC;    // normal in camera space
in vec4 v2f_color;

uniform bool useLighting;
uniform vec4 lightPositionC;  // light position in camera space
uniform vec4 lightColor;

struct Material {
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

uniform Material material;

out vec4 out_color;

void main() {
    if(useLighting) {
        // Compute the diffuse component
        vec4 N = normalize(v2f_normalC);
        vec4 L = normalize(lightPositionC - v2f_positionC);
        vec4 Diffuse =  max(dot(N, L), 0) * lightColor * material.diffuse;
         
        // Compute the specular component
        vec4 V = normalize(-v2f_positionC);
        vec4 H = normalize(L + V);
        vec4 Specular = pow(max(dot(H, N), 0), material.shininess) * lightColor * material.specular;
        
        // combine light components
        out_color = max(Diffuse + Specular, material.ambient);
    }
    else {
        //out_color = v2f_color;
        out_color = vec4(1.0, 1.0, 1.0, 1.0);
    }
} 