#include "headers/piece_mover.h"

PieceMover::PieceMover(int localScale) {
	this->localScale = localScale;
	this->calculatePositions();
}

bool PieceMover::movePiece(float time, bool is_white) {
    if (abs(this->distLeft[0]) < abs((this->dist[0] / 3)) * time || abs(this->distLeft[1]) < abs((this->dist[1] / 3) * time )) {
        this->currMat = glm::translate(this->currMat, glm::vec3(this->distLeft[0], 0, this->distLeft[1]));
        glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f));
        pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat;
        glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat));
        return true;
    }
    else {
        this->currMat = glm::translate(this->currMat, glm::vec3((this->dist[0]/3) * time, 0, (this->dist[1]/3) * time));
        this->distLeft[0] -= (this->dist[0] / 3) * time;
        this->distLeft[1] -= (this->dist[1] / 3) * time;
        glm::mat4 pieceMat = glm::scale(this->currMat, glm::vec3(0.8f, 0.8f, 0.8f));
        pieceMat = (is_white) ? glm::rotate(pieceMat, PI, glm::vec3(0.0f, 1.0f, 0.0f)) : pieceMat;
        glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, glm::value_ptr(pieceMat));
        return false;
    }
}

void PieceMover::setupMove(glm::mat4* matrix, std::string src_pos, std::string dest_pos) {
    float temp[] = { 0, 0 };
    float temp2[] = { 0, 0 };
    this->dist = temp;
    this->dist[0] = this->positions[dest_pos].first - this->positions[src_pos].first;
    this->dist[1] = this->positions[dest_pos].second - this->positions[src_pos].second;
    this->distLeft = temp2;
    this->distLeft[0] = this->positions[dest_pos].first - this->positions[src_pos].first;
    this->distLeft[1] = this->positions[dest_pos].second - this->positions[src_pos].second;
    this->srcPos = src_pos;
    this->destPos = dest_pos;
    this->currMat = glm::translate(*matrix, glm::vec3(this->positions[srcPos].first, 0.0f, this->positions[src_pos].second));
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