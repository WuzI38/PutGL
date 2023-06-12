/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define WINDOW_SIZE 1500 // TODO - IT SHOULD BE SCALABLE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "headers/model.h"
#include "headers/piece_mover.h"
#include "headers/constants.h"
#include "headers/shaderprogram.h"


char board[8][8] = {
	{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
	{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
	{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
	{'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
}; // reprezentacja planszy, dużel litery - czarne, małe - białe, x - puste
// refactor the code later to use this array

// Tekstury figur
GLuint boardTex;
GLuint tableTex;
GLuint whiteTex;
GLuint blackTex;

// Tekstury ścian i podłogi
GLuint floorTex;
GLuint wallTex;

float speed_y = 0; // prędkość obrotu kamery w poziomie
float speed_x = 0; // prędkość obrotu kamery w pionie
float moveTime = 0; // czas pomiędzy rysowaniem kolejnych klatek
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
// model figury która się rusza
Model* currModel;
// kolor figury która się rusza
bool currColor = true;
bool moveStarted = false;
bool moveEnded = true;
std::string line;
// zmienna tekstowa przechowująca reprezentację obecnie ruszającej się figury
char currPiece;
// wiersz do którego rusza się figura
int destRow;
// kolumna do której rusza się figura
int destCol;
// flaga reprezentująca to czy ruch jest roszadą
bool isCastling = false;
bool isEnPassant = false;
bool isUp = false;
bool isDown = false;

glm::mat4 castlingMat;
int srcColCastling;
int destColCastling;
float castlingDistance;

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}


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

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_UP) {
			speed_x = PI / 5; // jeżeli strzałka w górę to kamera przesuwa się w górę
		}
		if (key == GLFW_KEY_DOWN) {
			speed_x = - PI / 5; // jeżeli strzałka w dół kamera przesuwa się w dół
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



//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	// wczytanie tekstur
	boardTex = load_texture("textures/chessboard.png");
	tableTex = load_texture("textures/table.png");
	whiteTex = load_texture("textures/white.png");
	blackTex = load_texture("textures/black.png");

	// textury ścian i podłogi
	floorTex = load_texture("textures/floor.png");
	wallTex = load_texture("textures/wall.png");

    initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0.545, 1); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	glfwSetKeyCallback(window, key_callback); // włączenie obsługi przycisków
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


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
}

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

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_y, float angle_x) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości

	spTextured->use(); //Aktywuj program cieniujący, nie używający światła

	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 100.0f); //Wylicz macierz rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz widoku
	V = glm::rotate(V, angle_x, glm::vec3(0.0f, 0.0f, 1.0f)); // Obrót kamery w pione
	V = glm::rotate(V, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); // Obrót kamery w poziomie

	glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku

	glm::mat4 floorMat = glm::mat4(1.0f); // macierz jednostkowa
	drawMult(floorMat, floorPlane, 0.25f, 5); // rysowanie podłogi

	glm::mat4 wallMat = glm::mat4(1.0f); // macierz jednostkowa
	glm::mat4 wallMatP; // zmienna na macierz dla danej ściany
	for (int x = 0; x < 4; x++) {
		wallMatP = glm::rotate(wallMat, (PI / 2) * x, glm::vec3(0.0f, 1.0f, 0.0f));
		wallMatP = glm::translate(wallMatP, glm::vec3(6.25f, 0.0f, 0.0f));
		wallMatP = glm::rotate(wallMatP, (PI / 2), glm::vec3(0.0f, 0.0f, 1.0f));
		wallMatP = glm::rotate(wallMatP, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		drawMult(wallMatP, wallPlane, 0.25f, 5);
	}
	
	glm::mat4 tableMat = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową
	tableMat = glm::scale(tableMat, glm::vec3(1.1f, 1.1f, 1.1f)); //skalowanie stołu
	spLambertTextured->use(); // użycie programu cieniującego ze światłem
	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(tableMat)); // przekazanie do programu cieniującego macierzy dla stołu

	table->draw();

	glm::mat4 chessboardMat = glm::mat4(1.0f); // macierz jednostkowa
	chessboardMat = glm::translate(chessboardMat, glm::vec3(0.0f, 1.15f, 0.0f)); // przesunięcie planszy na stół
	chessboardMat = glm::rotate(chessboardMat, -PI / 2, glm::vec3(1.0f, 0.0f, 0.0f)); // obrócenie planszy
	chessboardMat = glm::scale(chessboardMat, glm::vec3(0.02f, 0.02f, 0.02f)); // przeskalowanie planszy

	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(chessboardMat)); // przekazanie macierzy do programu cieniującego

	chessboard->draw(); // narysowanie planszy

	glm::mat4 pieceMat = glm::mat4(1.0f); // macierz jednostkowa
	pieceMat = glm::translate(pieceMat, glm::vec3(0.0f, 1.147f, 0.0f)); // ustawienie figury na planszy
	pieceMat = glm::scale(pieceMat, glm::vec3(1.5f, 1.5f, 1.5f)); // przeskalowanie figury
	

	// proper move processing
	
	if (moveEnded && !moveStarted) {
		if (std::getline(infile, line)) {
			int col1 = (int)line.at(0) - (int)'a'; // wiersz pola z którego następuje ruch
			int row1 = line.at(1) - (int)'1'; // kolumna pola z którego następuje ruch
			destCol = (int)line.at(3) - (int)'a'; // przypisanie do zmiennej globalnej wiersza pola do którego następuje ruch
			destRow = line.at(4) - (int)'1'; // przypisanie do zmiennej globalnej kolumny pola do którego następuje ruch
			std::string destPos = line.substr(3, 2); // zmienna tekstowa reprezentująca pole do którego następuje ruch
			std::string srcPos = line.substr(0, 2); // zmienna tekstowa reprezentująca pole z którego następuje ruch
			move->setupMove(&pieceMat, srcPos, destPos); // wywołanie funkcji setupMove, wyliczenie dystansu który musi przebyć figura, takie rzeczy
			moveStarted = true; // ruch się zaczął
			moveEnded = false; // ruch się jeszcze nie skończył
			isUp = false; // model jeszcze się nie wzniósł
			isDown = false; // model jeszcze nie opadł
			char piece = board[row1][col1]; // zmienna używana w switchu, reprezentuje jaka figura zaczyna się poruszać
			if (piece == 'K' || piece == 'k') {
				isCastling = (abs(col1 - destCol) > 1) ? true : false; // sprawdzenie czy ruch jest roszadą, jeżeli król się rusza o więcej niż jedno pole to jest to roszada
			}
			
			currPiece = (line.at(6) == '0') ? board[row1][col1] : (islower(board[row1][col1])) ? tolower(line.at(6)) : line.at(6);; // zmienna przechowująca jaka figura się rusza (jak promocja to jaką się stanie po ruchu)
			if (tolower(currPiece) == 'p' && destCol != col1 && board[destRow][destCol] == 'x') {
				isEnPassant = true; // jeżeli pionek rusza się do sąsiedniej kolumny i pole na które się rusza jest puste oznacza to że występuje bicie w przelocie
			}
			else {
				isEnPassant = false;
			}
			// switch sprawdza jaki model pasuje do przesuwanej figury
			switch (piece)
			{
			case 'p':
				currModel = pawnWhite;
				currColor = true;
				break;
			case 'P':
				currModel = pawnBlack;
				currColor = false;
				break;
			case 'b':
				currModel = bishopWhite;
				currColor = true;
				break;
			case 'B':
				currModel = bishopBlack;
				currColor = false;
				break;
			case 'n':
				currModel = knightWhite;
				currColor = true;
				break;
			case 'N':
				currModel = knightBlack;
				currColor = false;
				break;
			case 'r':
				currModel = rookWhite;
				currColor = true;
				break;
			case 'R':
				currModel = rookBlack;
				currColor = false;
				break;
			case 'q':
				currModel = queenWhite;
				currColor = true;
				break;
			case 'Q':
				currModel = queenBlack;
				currColor = false;
				break;
			case 'k':
				currModel = kingWhite;
				currColor = true;
				break;
			case 'K':
				currModel = kingBlack;
				currColor = false;
				break;
			default:
				break;
			}
			board[row1][col1] = 'x';// pole z którego rusza się figura staje się puste

			if (isCastling) {
				srcColCastling = (destCol == 2) ? 0 : 7; // obliczenie kolumny z której rusza się wieża podczas roszady
				std::string tempCol = (srcColCastling == 0) ? "a" : "h"; // tymczasowa zmienna tekstowa reprezentująca kolumnę
				int tempRow = (currColor) ? 0 : 7; // cała funkcja jest tutaj gdyż dopiero w tym momencie znany jest kolor figury
				destColCastling = (destCol == 2) ? 3 : 5; // obliczenie kolumny do której będzie się poruszać wieża
				castlingDistance = move->calculateCastlingDistance(srcColCastling, destColCastling); // obliczenie odległości o którą należy przenieść wieżę
 				std::string field = std::string(1, (char)('a' + srcColCastling)) + std::to_string(tempRow+1); // tekstowa reprezentacja pola z którego rusza się wieża
				castlingMat = glm::translate(pieceMat, glm::vec3(move->getCol(field), 0.0f, move->getRow(field))); // wyznaczenie macieży dla początkowego pola wieży
			}
		}
	}
	// jeżeli ruch się zaczął a figura jeszcze nie opadła spowrotem na planszę
	if (moveStarted && !isDown) {
		if (!isUp) { // jeżeli figura jeszcze się nie wzniosła
			isUp = move->moveVertically(moveTime, currColor, true); // figura rusza się w pionie
		} else if(isUp && !moveEnded) { // jeżeli figura się wzniosła ale nie skończyła ruchu
			moveEnded = move->movePiece(moveTime, currColor); // ruch figury, wyliczenie macierzy
			
		}
		else if (isUp && moveEnded) { // jeżeli figura skończyła ruch 
			isDown = move->moveVertically(moveTime, currColor, false); // opuszczenie figury na planszę
		}
		currModel->draw(); // rysowanie modelu
		
		if (isUp && !moveEnded && isCastling) {
			board[destRow][srcColCastling] = 'x'; // usunięcie wieży z tablicy
			castlingMat = glm::translate(castlingMat, glm::vec3(moveTime * castlingDistance, 0.0f, 0.0f)); // z jakiegoś powodu liczę odległość między wierszami nie kolumnami, stąd minus
			glm::mat4 tempMat = glm::scale(castlingMat, glm::vec3(0.8f, 0.8f, 0.8f)); // utworzenie tymczasowej macierzy
			glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(tempMat)); 
			if (currColor) {
				rookWhite->draw();
			}
			else {
				rookBlack->draw();
			}
		}
		if (isUp && moveEnded && isCastling) { // ustawienie wieży na końcowej pozycji
			// jeżeli ruch był roszadą
			int tempCol = (destCol == 2) ? 3 : 5;
			int tempSrcCol = (destCol == 2) ? 0 : 7;
			board[destRow][tempCol] = (currColor) ? 'r' : 'R';
			board[destRow][tempSrcCol] = 'x';
			isCastling = false;
		}
	}
	if (isDown && moveStarted) { // jeżeli figura jest na planszy a ruch się zaczął
		moveStarted = false; // koniec ruchu
		board[destRow][destCol] = currPiece; // ustawienie figury na planszy
		if (isEnPassant) { // jeżeli bicie w przelocie
			int tempRow = (currColor) ? destRow - 1 : destRow + 1; // jeżeli figura jest biała to wiersz bitego pionka ma niższy numer, jak czarna to wyższy
			board[tempRow][destCol] = 'x'; // usunięcie bitego pionka znajdującego się pod obecną pozycją ruszanej figury
		}
	}
	// przechodzi przez całą tablicę i rysuje figury na odpowiednich polach
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			std::string pos = std::string(1, (char)'a' + j) + (char)((int) '1' + i);
			char piece = board[i][j];
			switch (piece)
			{
			case 'p':
				move->placePiece(&pieceMat, pos);
				pawnWhite->draw();
				break;
			case 'P':
				move->placePiece(&pieceMat, pos);
				pawnBlack->draw();
				break;
			case 'b':
				move->placePiece(&pieceMat, pos, true);
				bishopWhite->draw();
				break;
			case 'B':
				move->placePiece(&pieceMat, pos);
				bishopBlack->draw();
				break;
			case 'n':
				move->placePiece(&pieceMat, pos, true);
				knightWhite->draw();
				break;
			case 'N':
				move->placePiece(&pieceMat, pos);
				knightBlack->draw();
				break;
			case 'r':
				move->placePiece(&pieceMat, pos, true);
				rookWhite->draw();
			case 'R':
				move->placePiece(&pieceMat, pos);
				rookBlack->draw();
				break;
			case 'q':
				move->placePiece(&pieceMat, pos, true);
				queenWhite->draw();
				break;
			case 'Q':
				move->placePiece(&pieceMat, pos);
				queenBlack->draw();
				break;
			case 'k':
				move->placePiece(&pieceMat, pos, true);
				kingWhite->draw();
				break;
			case 'K':
				move->placePiece(&pieceMat, pos);
				kingBlack->draw();
				break;
			default:
				break;
			}
		}
	}
	
	glfwSwapBuffers(window); //Skopiuj bufor tylny do bufora przedniego
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "Chess", NULL, NULL); 

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące
	//Główna pętla
	
	glfwSetTime(0); //Wyzeruj licznik czasu
	float angle_y = 0; // kąt obrotu kamery w poziomie
	float angle_x = 0; // kąt obrotu kamery w pionie
	
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		moveTime = glfwGetTime();
		angle_y += glfwGetTime() * speed_y; // wylicza kąt obrotu kamery w poziomie
		angle_y = (angle_y > 2 * PI) ? 0 : (angle_y < - 2 * PI) ? 0 : angle_y; // ograniczenie, jeżeli wartość przekroczy 360 stopni resetuje się na 0 (aby uniknąć overflow)
		angle_x += glfwGetTime() * speed_x; // wylicza kąt obrotu kamery w pionie
		angle_x = (angle_x < -4 * PI / 10) ? -4 * PI / 10 : (angle_x > 0) ? 0 : angle_x;  // ograniczenie, tak samo jak wyżej, kamera patrzy na stolik maksymalnie od boku albo od góry
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, angle_y, angle_x); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}