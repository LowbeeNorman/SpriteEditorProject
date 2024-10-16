// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#include "eraser.h"

Eraser::Eraser (Model &model) : model (model) {}

void Eraser::action (Action act, int x, int y, int size) {
    Q_UNUSED (act);

    for (int i = x - size / 2; i <= x + size / 2; i++){ // Loop between all possible sizes
        for (int j = y - size / 2; j <= y + size / 2; j++){ // Loop between all possible sizes
            model.setPixel (i, j, Qt::transparent);
        }
    }

}
