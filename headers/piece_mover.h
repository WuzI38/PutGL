#ifndef PIECE_MOVER_H
#define PIECE_MOVER_H

#pragma once // // I'm unsure of which ones are useless, but some of them are.
#include <iostream>
#include <vector>
#include "headers/shaderprogram.h"
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
	virtual bool moveVertically(float time, bool is_white, bool up = true);
	virtual float calculateCastlingDistance(int srcCol, int destCol);
	virtual float getRow(std::string field);
	virtual float getCol(std::string field);


private:
	std::map<std::string, std::pair<float, float>> positions;
	float localScale;
	std::string srcPos; // reprezentacja tekstowa pola z kt�rego wykonuje si� ruch
	std::string destPos; // reprezentacja tekstowa pola do kt�rego wykonuje si� ruch
	glm::mat4 currMat; // aktualna macierz przesuwanej figury
	float dist[2]; // dystans do pokonania mi�dzy srcPos a destPos
	float distLeft[2]; // dystans jaki zosta� do pokonania
	float verticalDist[2]; // dystans jaki zosta� do pokonania w pionie [0] - w g�r�, [1] - w d�
	float vD;
	virtual void calculatePositions();
};

#endif
