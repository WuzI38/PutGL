#ifndef MODEL_H
#define MODEL_H

#pragma once // SOME OF THESE ARE USELESS, I JUST HAVE NO IDEA WHICH EXACTLY
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "headers/lodepng.h"
#include "headers/shaderprogram.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

class Model {
public:
	Model(const char* modelPath, const char* texturePath);
	virtual ~Model();
	virtual void draw();

private:
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> normals;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> indices;
	GLuint texture;

	virtual void loadModel(std::string filepath);
	virtual void loadTexture(std::string filepath);
};

#endif
