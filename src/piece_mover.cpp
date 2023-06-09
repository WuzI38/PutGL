#include "headers/piece_mover.h"

PieceMover::PieceMover(int localScale) {
	this->localScale = localScale;
	this->calculatePositions();
}

bool PieceMover::movePiece(float time, bool is_white) {
    // je�eli odleg�o�� jak� mia�aby pokona� figura jest wi�ksza od tej jaka zosta�a do wyliczonego punktu
    if (abs(this->distLeft[0]) < abs((this->dist[0])) * time || abs(this->distLeft[1]) < abs((this->dist[1]) * time )) {
        this->currMat = glm::translate(this->currMat, glm::vec3(this->distLeft[0], 0, this->distLeft[1])); // translacja macierzy o pozosta�y dystans
        glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f)); // przeskalowanie modelu o 4/5 (lepiej si� mie�ci na polach)
        pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat; // je�eli figura jest bia�a obraca si� o 180 stopni
        glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat)); // wprowadza macierz modelu do vertex shadera
        return true; // ruch si� sko�czy�
    }
    // w przeciwnym wypadku
    else {
        this->currMat = glm::translate(this->currMat, glm::vec3((this->dist[0]) * time, 0, (this->dist[1]) * time)); // translacja macierzy o wyliczony dystans
        this->distLeft[0] -= (this->dist[0]) * time; // zmniejsza dystans pozosta�y do pokonania na osi x o ten w�a�nie pokonany
        this->distLeft[1] -= (this->dist[1]) * time; // zmniejsza dystans pozosta�y do pokonania na osi y o ten w�a�nie pokonany
        glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f)); // przeskalowanie modelu o 4/5 (lepiej si� mie�ci na polach)
        pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat; // je�eli figura jest bia�a obraca si� o 180 stopni
        glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat));// wprowadza macierz modelu do vertex shadera
        return false; // ruch si� jeszcze nie sko�czy�
    }
}

void PieceMover::setupMove(glm::mat4* matrix, std::string src_pos, std::string dest_pos) {
    this->dist[0] = this->positions[dest_pos].first - this->positions[src_pos].first; // wylicza dystans mi�dzy pierwsz� wsp�rz�dn� pola z kt�rego i pola do kt�rego wykonywany jest ruch
    this->dist[1] = this->positions[dest_pos].second - this->positions[src_pos].second; // wylicza dystans mi�dzy drug� wsp�rz�dn� pola z kt�rego i pola do kt�rego wykonywany jest ruch
    this->distLeft[0] = this->positions[dest_pos].first - this->positions[src_pos].first; // wylicza dystans mi�dzy pierwsz� wsp�rz�dn� pola z kt�rego i pola do kt�rego wykonywany jest ruch
    this->distLeft[1] = this->positions[dest_pos].second - this->positions[src_pos].second; // wylicza dystans mi�dzy drug� wsp�rz�dn� pola z kt�rego i pola do kt�rego wykonywany jest ruch
    this->verticalDist[0] = 0.1;
    this->verticalDist[1] = -0.1;
    this->srcPos = src_pos; // reprezentacja tekstowa pola z kt�rego wykonuje si� ruch
    this->destPos = dest_pos; // reprezentacja tekstowa pola do kt�rego wykonuje si� ruch
    this->currMat = glm::translate(*matrix, glm::vec3(this->positions[srcPos].first, 0.0f, this->positions[src_pos].second)); // pocz�tkowa macierz przesuwanej figury - macierz wskazuj�ca na pole z kt�rego wykonywany jest ruch
}

bool PieceMover::moveVertically(float time,bool is_white,  bool up) {
    if (up) {
        if (abs(0.1 * 2 * time) > abs(this->verticalDist[0])) {
            this->currMat = glm::translate(this->currMat, glm::vec3(0, this->verticalDist[0], 0)); // translacja macierzy o pozosta�y dystans
            glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f)); // przeskalowanie modelu o 4/5 (lepiej si� mie�ci na polach)
            pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat; // je�eli figura jest bia�a obraca si� o 180 stopni
            glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat)); // wprowadza macierz modelu do vertex shadera
            return true;
        }
        else {
            this->verticalDist[0] -= 0.1 * 2 * time;
            this->currMat = glm::translate(this->currMat, glm::vec3(0, 0.1 * 2 * time, 0)); // translacja macierzy o pozosta�y dystans
            glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f)); // przeskalowanie modelu o 4/5 (lepiej si� mie�ci na polach)
            pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat; // je�eli figura jest bia�a obraca si� o 180 stopni
            glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat)); // wprowadza macierz modelu do vertex shadera
            return false;
        }
    }
    else {
        if (abs(0.1 * 2 * time) > abs(this->verticalDist[1])) {
            this->currMat = glm::translate(this->currMat, glm::vec3(0, this->verticalDist[1], 0)); // translacja macierzy o pozosta�y dystans
            glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f)); // przeskalowanie modelu o 4/5 (lepiej si� mie�ci na polach)
            pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat; // je�eli figura jest bia�a obraca si� o 180 stopni
            glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat)); // wprowadza macierz modelu do vertex shadera
            return true;
        }
        else {
            this->verticalDist[1] -= -0.1 * 2 * time;
            this->currMat = glm::translate(this->currMat, glm::vec3(0, -0.1 * 2 * time, 0)); // translacja macierzy o pozosta�y dystans
            glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f)); // przeskalowanie modelu o 4/5 (lepiej si� mie�ci na polach)
            pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat; // je�eli figura jest bia�a obraca si� o 180 stopni
            glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat)); // wprowadza macierz modelu do vertex shadera
            return false;
        }
    }
}

void PieceMover::placePiece(glm::mat4* matrix, std::string field, bool is_white) {
    glm::mat4 pieceMat = glm::translate(*matrix, glm::vec3(this->positions[field].first, 0.0f, this->positions[field].second));
    pieceMat = glm::scale(pieceMat, glm::vec3(0.8f, 0.8f, 0.8f));
    pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat;
    //std::cout << this->positions[field].first << " " << this->positions[field].second << std::endl;
    glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat));
}

void PieceMover::calculatePositions() {
    int letterIndex = 1;
    const float scaleConst = 42000; // TOTALLY RANDOM VALUE, SHOULD BE MODIFIED LATER I GUESS

    for (char letter = 'a'; letter <= 'h'; ++letter) {
        // Iterate through numbers from 1 to 8
        int x = 2 * letterIndex - 9; // Strange chessboard position calculations v2
        for (int number = 1; number <= 8; ++number) {
            // Create the key by concatenating the letter and number
            std::string key = std::string(1, letter) + std::to_string(number);

            int y = 9 - 2 * number; // Strange chessboard position calculations

            this->positions[key] = std::make_pair(((float)this->localScale / scaleConst) * x, 
                                            ((float)this->localScale / scaleConst) * y);
        }
        letterIndex++;
    }
}