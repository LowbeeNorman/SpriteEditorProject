// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#include "pen.h"

Pen::Pen (Model &model) : model (model) {}

void Pen::action (Action act, int x, int y, int size) {
    Q_UNUSED (act);

    for (int i = x - size / 2; i <= x + size / 2; i++){ // Loop between all possible tool sizes
        for (int j = y - size / 2; j <= y + size / 2; j++){ // Loop between all possible tool sizes
            model.setPixel (i, j);
        }
    }
}
