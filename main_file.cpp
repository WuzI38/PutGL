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

float speed_y = 0; // prędkość obrotu kamery w poziomie
float speed_x = 0; // prędkość obrotu kamery w pionie
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


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
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
			speed_y = PI / 4;
		}
		if (key == GLFW_KEY_LEFT) {
			speed_y = -PI / 4;
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
    initShaders();
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(1, 1, 1, 0.6); //Ustaw kolor czyszczenia bufora kolorów
	glEnable(GL_DEPTH_TEST); //Włącz test głębokości na pikselach
	glfwSetKeyCallback(window, key_callback);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	move = new PieceMover(WINDOW_SIZE);

	chessboard = new Model("objects/chessboard.obj", "textures/chessboard.png");
	table = new Model("objects/table.obj", "textures/table.png");

	pawnWhite = new Model("objects/pawn.obj", "textures/white.png");
	pawnBlack = new Model("objects/pawn.obj", "textures/black.png");

	rookWhite = new Model("objects/rook.obj", "textures/white.png");
	rookBlack = new Model("objects/rook.obj", "textures/black.png");

	knightWhite = new Model("objects/knight.obj", "textures/white.png");
	knightBlack = new Model("objects/knight.obj", "textures/black.png");

	bishopWhite = new Model("objects/bishop.obj", "textures/white.png");
	bishopBlack = new Model("objects/bishop.obj", "textures/black.png");

	queenWhite = new Model("objects/queen.obj", "textures/white.png");
	queenBlack = new Model("objects/queen.obj", "textures/black.png");

	kingWhite = new Model("objects/king.obj", "textures/white.png");
	kingBlack = new Model("objects/king.obj", "textures/black.png");
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
    freeShaders();
}


//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_y, float angle_x) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyść bufor koloru i bufor głębokości

	spLambertTextured->use(); //Aktywuj program cieniujący

	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 100.0f); //Wylicz macierz rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz widoku
	V = glm::rotate(V, angle_x, glm::vec3(0.0f, 0.0f, 1.0f)); // Obrót kamery w pione
	V = glm::rotate(V, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); // Obrót kamery w poziomie

	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P)); //Załaduj do programu cieniującego macierz rzutowania
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V)); //Załaduj do programu cieniującego macierz widoku
	
	glm::mat4 tableMat = glm::mat4(1.0f); //Zainicjuj macierz modelu macierzą jednostkową
	tableMat = glm::scale(tableMat, glm::vec3(1.1f, 1.1f, 1.1f)); //Pomnóż macierz modelu razy macierz obrotu o kąt angle wokół osi X

	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(tableMat));

	table->draw();

	glm::mat4 chessboardMat = glm::mat4(1.0f);
	chessboardMat = glm::translate(chessboardMat, glm::vec3(0.0f, 1.2f, 0.0f));
	chessboardMat = glm::rotate(chessboardMat, -PI / 2, glm::vec3(1.0f, 0.0f, 0.0f));
	chessboardMat = glm::scale(chessboardMat, glm::vec3(0.02f, 0.02f, 0.02f));

	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(chessboardMat));

	chessboard->draw();

	glm::mat4 pieceMat = glm::mat4(1.0f);
	pieceMat = glm::translate(pieceMat, glm::vec3(0.0f, 1.2f, 0.0f));
	pieceMat = glm::scale(pieceMat, glm::vec3(1.5f, 1.5f, 1.5f));

	// Move every piece to its position

	for (char letter = 'a'; letter <= 'h'; ++letter) {
		std::string whitePos = std::string(1, letter) + "7";
		move->placePiece(&pieceMat, whitePos);
		pawnWhite->draw();
		std::string blackPos = std::string(1, letter) + "2";
		move->placePiece(&pieceMat, blackPos);
		pawnBlack->draw();

		whitePos = std::string(1, letter) + "8";
		blackPos = std::string(1, letter) + "1";

		if (letter == 'a' || letter == 'h') { // THIS THING CAN BE OF COURSE OPTIMIZED... 
			move->placePiece(&pieceMat, whitePos); // <- YEP, NO REDUNDANCY AT ALL
			rookWhite->draw();
			move->placePiece(&pieceMat, blackPos);
			rookBlack->draw();
		}

		if (letter == 'b' || letter == 'g') {
			move->placePiece(&pieceMat, whitePos); // <- YEP, NO REDUNDANCY AT ALL
			knightWhite->draw();
			move->placePiece(&pieceMat, blackPos);
			knightBlack->draw();
		}

		if (letter == 'c' || letter == 'f') {
			move->placePiece(&pieceMat, whitePos);
			bishopWhite->draw();
			move->placePiece(&pieceMat, blackPos);
			bishopBlack->draw();
		}

		if (letter == 'd') {
			move->placePiece(&pieceMat, whitePos);
			queenWhite->draw();
			move->placePiece(&pieceMat, blackPos);
			queenBlack->draw();
		}

		if (letter == 'e') {
			move->placePiece(&pieceMat, whitePos);
			kingWhite->draw();
			move->placePiece(&pieceMat, blackPos);
			kingBlack->draw();
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

	window = glfwCreateWindow(WINDOW_SIZE, WINDOW_SIZE, "Chess", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

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
