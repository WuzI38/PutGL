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
#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include "headers/const.h"
#include "headers/model.h"
#include "headers/piece_mover.h"
#include "headers/shaderprogram.h"
#include "headers/globals.h"
#include "headers/utils.h"
#include "headers/scene.h"

// Main procedure
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