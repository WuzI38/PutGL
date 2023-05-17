#ifndef PIECE_MOVER_H
#define PIECE_MOVER_H

#pragma once // SOME OF THESE ARE USELESS, I JUST HAVE NO IDEA WHICH EXACTLY
#include <iostream>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "headers/lodepng.h"
#include "headers/shaderprogram.h"
#include "headers/constants.h"
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <string>
#include <utility>


class PieceMover {
public:
	PieceMover(int localScale);
	virtual void movePiece(glm::mat4* matrix, std::string field); // Fluent movement - not implemented
	virtual void placePiece(glm::mat4* matrix, std::string field, bool is_white=false); // Teleport a piece to given position

private:
	std::map<std::string, std::pair<float, float>> positions;
	float localScale;

	virtual void calculatePositions();
};

#endif
