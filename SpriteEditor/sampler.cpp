// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#include "sampler.h"

Sampler::Sampler (Model &model) : model (model) {}

void Sampler::action (Action act, int x, int y, int size = 1) {
    if (act != Action::start)
        return;
    model.shockAndAwe (size * x, size * y);
}
