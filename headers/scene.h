#ifndef SCENE_H
#define SCENE_H

#pragma once

#include "globals.h"
#include "utils.h"

//Procedura rysuj�ca zawarto�� sceny
void drawScene(GLFWwindow* window, float angle_y, float angle_x) {
	//************Tutaj umieszczaj kod rysuj�cy obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczy�� bufor koloru i bufor g��boko�ci

	spTextured->use(); //Aktywuj program cieniuj�cy, nie u�ywaj�cy �wiat�a

	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 100.0f); //Wylicz macierz rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); //Wylicz macierz widoku
	V = glm::rotate(V, angle_x, glm::vec3(0.0f, 0.0f, 1.0f)); // Obr�t kamery w pione
	V = glm::rotate(V, angle_y, glm::vec3(0.0f, 1.0f, 0.0f)); // Obr�t kamery w poziomie

	glUniformMatrix4fv(spTextured->u("P"), 1, false, glm::value_ptr(P)); //Za�aduj do programu cieniuj�cego macierz rzutowania
	glUniformMatrix4fv(spTextured->u("V"), 1, false, glm::value_ptr(V)); //Za�aduj do programu cieniuj�cego macierz widoku

	glm::mat4 floorMat = glm::mat4(1.0f); // macierz jednostkowa
	drawMult(floorMat, floorPlane, 0.25f, 5); // rysowanie pod�ogi

	glm::mat4 wallMat = glm::mat4(1.0f); // macierz jednostkowa
	glm::mat4 wallMatP; // zmienna na macierz dla danej �ciany
	for (int x = 0; x < 4; x++) {
		wallMatP = glm::rotate(wallMat, (PI / 2) * x, glm::vec3(0.0f, 1.0f, 0.0f));
		wallMatP = glm::translate(wallMatP, glm::vec3(6.25f, 0.0f, 0.0f));
		wallMatP = glm::rotate(wallMatP, (PI / 2), glm::vec3(0.0f, 0.0f, 1.0f));
		wallMatP = glm::rotate(wallMatP, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
		drawMult(wallMatP, wallPlane, 0.25f, 5);
	}

	glm::mat4 tableMat = glm::mat4(1.0f); //Zainicjuj macierz modelu macierz� jednostkow�
	tableMat = glm::scale(tableMat, glm::vec3(1.1f, 1.1f, 1.1f)); //skalowanie sto�u
	spLambertTextured->use(); // u�ycie programu cieniuj�cego ze �wiat�em
	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P)); //Za�aduj do programu cieniuj�cego macierz rzutowania
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V)); //Za�aduj do programu cieniuj�cego macierz widoku
	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(tableMat)); // przekazanie do programu cieniuj�cego macierzy dla sto�u

	table->draw();

	glm::mat4 chessboardMat = glm::mat4(1.0f); // macierz jednostkowa
	chessboardMat = glm::translate(chessboardMat, glm::vec3(0.0f, 1.15f, 0.0f)); // przesuni�cie planszy na st�
	chessboardMat = glm::rotate(chessboardMat, -PI / 2, glm::vec3(1.0f, 0.0f, 0.0f)); // obr�cenie planszy
	chessboardMat = glm::scale(chessboardMat, glm::vec3(0.02f, 0.02f, 0.02f)); // przeskalowanie planszy

	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(chessboardMat)); // przekazanie macierzy do programu cieniuj�cego

	chessboard->draw(); // narysowanie planszy

	glm::mat4 pieceMat = glm::mat4(1.0f); // macierz jednostkowa
	pieceMat = glm::translate(pieceMat, glm::vec3(0.0f, 1.147f, 0.0f)); // ustawienie figury na planszy
	pieceMat = glm::scale(pieceMat, glm::vec3(1.5f, 1.5f, 1.5f)); // przeskalowanie figury


	// proper move processing

	if (moveEnded && !moveStarted) {
		if (std::getline(infile, line)) {
			int col1 = (int)line.at(0) - (int)'a'; // wiersz pola z kt�rego nast�puje ruch
			int row1 = line.at(1) - (int)'1'; // kolumna pola z kt�rego nast�puje ruch
			destCol = (int)line.at(3) - (int)'a'; // przypisanie do zmiennej globalnej wiersza pola do kt�rego nast�puje ruch
			destRow = line.at(4) - (int)'1'; // przypisanie do zmiennej globalnej kolumny pola do kt�rego nast�puje ruch
			std::string destPos = line.substr(3, 2); // zmienna tekstowa reprezentuj�ca pole do kt�rego nast�puje ruch
			std::string srcPos = line.substr(0, 2); // zmienna tekstowa reprezentuj�ca pole z kt�rego nast�puje ruch
			move->setupMove(&pieceMat, srcPos, destPos); // wywo�anie funkcji setupMove, wyliczenie dystansu kt�ry musi przeby� figura, takie rzeczy
			moveStarted = true; // ruch si� zacz��
			moveEnded = false; // ruch si� jeszcze nie sko�czy�
			isUp = false; // model jeszcze si� nie wzni�s�
			isDown = false; // model jeszcze nie opad�
			char piece = board[row1][col1]; // zmienna u�ywana w switchu, reprezentuje jaka figura zaczyna si� porusza�
			if (piece == 'K' || piece == 'k') {
				isCastling = (abs(col1 - destCol) > 1) ? true : false; // sprawdzenie czy ruch jest roszad�, je�eli kr�l si� rusza o wi�cej ni� jedno pole to jest to roszada
			}

			currPiece = (line.at(6) == '0') ? board[row1][col1] : (islower(board[row1][col1])) ? tolower(line.at(6)) : line.at(6);; // zmienna przechowuj�ca jaka figura si� rusza (jak promocja to jak� si� stanie po ruchu)
			if (tolower(currPiece) == 'p' && destCol != col1 && board[destRow][destCol] == 'x') {
				isEnPassant = true; // je�eli pionek rusza si� do s�siedniej kolumny i pole na kt�re si� rusza jest puste oznacza to �e wyst�puje bicie w przelocie
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
			board[row1][col1] = 'x';// pole z kt�rego rusza si� figura staje si� puste

			if (isCastling) {
				srcColCastling = (destCol == 2) ? 0 : 7; // obliczenie kolumny z kt�rej rusza si� wie�a podczas roszady
				std::string tempCol = (srcColCastling == 0) ? "a" : "h"; // tymczasowa zmienna tekstowa reprezentuj�ca kolumn�
				int tempRow = (currColor) ? 0 : 7; // ca�a funkcja jest tutaj gdy� dopiero w tym momencie znany jest kolor figury
				destColCastling = (destCol == 2) ? 3 : 5; // obliczenie kolumny do kt�rej b�dzie si� porusza� wie�a
				castlingDistance = move->calculateCastlingDistance(srcColCastling, destColCastling); // obliczenie odleg�o�ci o kt�r� nale�y przenie�� wie��
				std::string field = std::string(1, (char)('a' + srcColCastling)) + std::to_string(tempRow + 1); // tekstowa reprezentacja pola z kt�rego rusza si� wie�a
				castlingMat = glm::translate(pieceMat, glm::vec3(move->getCol(field), 0.0f, move->getRow(field))); // wyznaczenie macie�y dla pocz�tkowego pola wie�y
			}
		}
	}
	// je�eli ruch si� zacz�� a figura jeszcze nie opad�a spowrotem na plansz�
	if (moveStarted && !isDown) {
		if (!isUp) { // je�eli figura jeszcze si� nie wznios�a
			isUp = move->moveVertically(moveTime * ANIM_SPEED, currColor, true); // figura rusza si� w pionie
		}
		else if (isUp && !moveEnded) { // je�eli figura si� wznios�a ale nie sko�czy�a ruchu
			moveEnded = move->movePiece(moveTime * ANIM_SPEED, currColor); // ruch figury, wyliczenie macierzy

		}
		else if (isUp && moveEnded) { // je�eli figura sko�czy�a ruch 
			board[destRow][destCol] = 'x'; // I GUESS HERE (now you see meeee.... NOW YOU DONT!!!)
			isDown = move->moveVertically(moveTime * ANIM_SPEED, currColor, false); // opuszczenie figury na plansz�
		}
		currModel->draw(); // rysowanie modelu

		if (isUp && !moveEnded && isCastling) {
			board[destRow][srcColCastling] = 'x'; // usuni�cie wie�y z tablicy
			castlingMat = glm::translate(castlingMat, glm::vec3(moveTime * ANIM_SPEED * castlingDistance, 0.0f, 0.0f)); // z jakiego� powodu licz� odleg�o�� mi�dzy wierszami nie kolumnami, st�d minus
			glm::mat4 tempMat = glm::scale(castlingMat, glm::vec3(0.8f, 0.8f, 0.8f)); // utworzenie tymczasowej macierzy
			glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(tempMat));
			if (currColor) {
				rookWhite->draw();
			}
			else {
				rookBlack->draw();
			}
		}
		if (isUp && moveEnded && isCastling) { // ustawienie wie�y na ko�cowej pozycji
			// je�eli ruch by� roszad�
			int tempCol = (destCol == 2) ? 3 : 5;
			int tempSrcCol = (destCol == 2) ? 0 : 7;
			board[destRow][tempCol] = (currColor) ? 'r' : 'R';
			board[destRow][tempSrcCol] = 'x';
			isCastling = false;
		}
	}
	if (isDown && moveStarted) { // je�eli figura jest na planszy a ruch si� zacz��
		moveStarted = false; // koniec ruchu
		board[destRow][destCol] = currPiece; // ustawienie figury na planszy
		if (isEnPassant) { // je�eli bicie w przelocie
			int tempRow = (currColor) ? destRow - 1 : destRow + 1; // je�eli figura jest bia�a to wiersz bitego pionka ma ni�szy numer, jak czarna to wy�szy
			board[tempRow][destCol] = 'x'; // usuni�cie bitego pionka znajduj�cego si� pod obecn� pozycj� ruszanej figury
		}
	}
	// przechodzi przez ca�� tablic� i rysuje figury na odpowiednich polach
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			std::string pos = std::string(1, (char)'a' + j) + (char)((int)'1' + i);
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

#endif