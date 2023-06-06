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
	virtual bool movePiece(float time, bool is_white); // Fluent movement
	virtual void setupMove(glm::mat4* matrix, std::string src_pos, std::string dest_pos); // Setup for fluent movement
	virtual void placePiece(glm::mat4* matrix, std::string field, bool is_white=false); // Teleport piece to a given position


private:
	std::map<std::string, std::pair<float, float>> positions;
	float localScale;
	std::string srcPos; // reprezentacja tekstowa pola z którego wykonuje siê ruch
	std::string destPos; // reprezentacja tekstowa pola do którego wykonuje siê ruch
	glm::mat4 currMat; // aktualna macierz przesuwanej figury
	float dist[2]; // dystans do pokonania miêdzy srcPos a destPos
	float distLeft[2]; // dystans jaki zosta³ do pokonania
	virtual void calculatePositions();
};

#endif
