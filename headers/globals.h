#ifndef GLOBALS_H
#define GLOBALS_H

#pragma once
#include <fstream>
#include <iostream>
#include "headers/model.h"

char board[8][8] = {
	{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
	{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
	{'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
}; // reprezentacja planszy, du¿el litery - czarne, ma³e - bia³e, x - puste
// refactor the code later to use this array

// Tekstury figur
GLuint boardTex;
GLuint tableTex;
GLuint whiteTex;
GLuint blackTex;

// Tekstury œcian i pod³ogi
GLuint floorTex;
GLuint wallTex;

float speed_y = 0; // prêdkoœæ obrotu kamery w poziomie
float speed_x = 0; // prêdkoœæ obrotu kamery w pionie
float moveTime = 0; // czas pomiêdzy rysowaniem kolejnych klatek
PieceMover* move;

Model* chessboard;
Model* table;

Model* pawnWhite;
Model* pawnBlack;

Model* rookWhite;
Model* rookBlack;

Model* knightWhite;
Model* knightBlack;

Model* bishopWhite;
Model* bishopBlack;

Model* queenWhite;
Model* queenBlack;

Model* kingWhite;
Model* kingBlack;

Model* floorPlane;
Model* wallPlane;

std::ifstream infile("games/game.csv");

// Zmienne globalne do ruchu figur
// model figury która siê rusza
Model* currModel;

// kolor figury która siê rusza
bool currColor = true;
bool moveStarted = false;
bool moveEnded = true;

std::string line;

// zmienna tekstowa przechowuj¹ca reprezentacjê obecnie ruszaj¹cej siê figury
char currPiece;
// wiersz do którego rusza siê figura
int destRow;
// kolumna do której rusza siê figura
int destCol;

// flaga reprezentuj¹ca to czy ruch jest roszad¹
bool isCastling = false;
bool isEnPassant = false;
bool isUp = false;
bool isDown = false;

glm::mat4 castlingMat;
int srcColCastling;
int destColCastling;
float castlingDistance;

#endif