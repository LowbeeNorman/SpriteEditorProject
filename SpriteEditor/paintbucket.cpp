// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#include "paintbucket.h"

PaintBucket::PaintBucket (Model &model) : model (model) {}

void PaintBucket::action (Action act, int x, int y, int size = 1) {
    if (act == Action::start && model.getCurrentColor () != model.getPixel (size * x,size * y)) {
        floodFill (x, y, model.getPixel (x, y));
    }
}

void PaintBucket::floodFill (int x, int y, QRgb targetColor) {
    if (model.getPixel (x, y) != targetColor
        || x < 0 || y < 0
        || x >= model.getSize ().width () || y >= model.getSize ().height ())
        return;
    model.setPixel (x, y);

    floodFill (x, y + 1, targetColor);
    floodFill (x, y - 1, targetColor);
    floodFill (x - 1, y, targetColor);
    floodFill (x + 1, y, targetColor);
}
