#include "headers/piece_mover.h"

PieceMover::PieceMover(int localScale) {
	this->localScale = localScale;
	this->calculatePositions();
}

void PieceMover::movePiece(glm::mat4* matrix, std::string field) {
	;
}


void PieceMover::placePiece(glm::mat4* matrix, std::string field) {
    glm::mat4 pieceMat = glm::translate(*matrix, glm::vec3(this->positions[field].first, 0.0f, this->positions[field].second));
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