#version 330


out vec4 FragColor; // zmienna wyj�ciowa

in vec3 TexCoords; // koordynaty teksturowania 

uniform samplerCube skybox; // tekstura skyboxa

void main()
{    
    FragColor = texture(skybox, TexCoords); // oblicznie koloru
}