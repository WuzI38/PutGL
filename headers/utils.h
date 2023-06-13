#ifndef UTILS_H
#define UTILS_H

#pragma once

#include "globals.h"

// £adowanie tekstur
GLuint load_texture(std::string filepath) {
	GLuint tex;
	// Load to memory
	std::vector<unsigned char> image;
	unsigned width, height;
	// load image
	unsigned error = lodepng::decode(image, width, height, filepath);
	// Import to GPU memory
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex); // Init handle
	glBindTexture(GL_TEXTURE_2D, tex); // Activate handle
	// Load image + handle
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return tex;
}

// Obs³uga klawiszy
void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_UP) {
			speed_x = PI / 5; // je¿eli strza³ka w górê to kamera przesuwa siê w górê
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = -PI / 5; // je¿eli strza³ka w dó³ kamera przesuwa siê w dó³
		}
		if (key == GLFW_KEY_RIGHT) {
			speed_y = -PI / 4;
		}
		if (key == GLFW_KEY_LEFT) {
			speed_y = PI / 4;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
			speed_y = 0;
		}
		if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
			speed_x = 0;
		}
	}
}

//Procedura inicjuj¹ca
void initOpenGLProgram(GLFWwindow* window) {
	// wczytanie tekstur
	boardTex = load_texture("textures/chessboard.png");
	tableTex = load_texture("textures/table.png");
	whiteTex = load_texture("textures/white.png");
	blackTex = load_texture("textures/black.png");

	// textury œcian i pod³ogi
	floorTex = load_texture("textures/floor.png");
	wallTex = load_texture("textures/wall.png");

	initShaders();
	//************Tutaj umieszczaj kod, który nale¿y wykonaæ raz, na pocz¹tku programu************
	glClearColor(0, 0, 0.545, 1); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //W³¹cz test g³êbokoœci na pikselach
	glfwSetKeyCallback(window, key_callback); // w³¹czenie obs³ugi przycisków
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	move = new PieceMover(WINDOW_SIZE);

	// wczytanie modeli
	chessboard = new Model("objects/chessboard.obj", boardTex);
	table = new Model("objects/table.obj", tableTex);

	pawnWhite = new Model("objects/pawn.obj", whiteTex);
	pawnBlack = new Model("objects/pawn.obj", blackTex);

	rookWhite = new Model("objects/rook.obj", whiteTex);
	rookBlack = new Model("objects/rook.obj", blackTex);

	knightWhite = new Model("objects/knight.obj", whiteTex);
	knightBlack = new Model("objects/knight.obj", blackTex);

	bishopWhite = new Model("objects/bishop.obj", whiteTex);
	bishopBlack = new Model("objects/bishop.obj", blackTex);

	queenWhite = new Model("objects/queen.obj", whiteTex);
	queenBlack = new Model("objects/queen.obj", blackTex);

	kingWhite = new Model("objects/king.obj", whiteTex);
	kingBlack = new Model("objects/king.obj", blackTex);

	floorPlane = new Model("objects/plane.obj", floorTex);
	wallPlane = new Model("objects/plane.obj", wallTex);
}

//Zwolnienie zasobów zajêtych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
}

//Procedura obs³ugi b³êdów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

// Rysowanie du¿ych elementów z³o¿onych z elementów w mniejszym stopniu du¿ych
void drawMult(glm::mat4& matrix, Model* model, float scale = 0.25f, int count = 3) {
	const float planeSize = 10.0f;
	count = (count - (count % 2)) / 2;
	matrix = glm::scale(matrix, glm::vec3(scale, scale, scale));
	glm::mat4 matTrans;
	for (float x = -planeSize * count; x <= planeSize * count; x += planeSize) {
		for (float z = -planeSize * count; z <= planeSize * count; z += planeSize) {
			matTrans = glm::translate(matrix, glm::vec3(x, 0.0f, z));
			glUniformMatrix4fv(spTextured->u("M"), 1, false, glm::value_ptr(matTrans));
			model->draw();
		}
	}
}

#endif