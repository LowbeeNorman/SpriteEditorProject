// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#ifndef SAMPLER_H
#define SAMPLER_H

#include "tool.h"
#include "model.h"

///
/// \brief This class represents the color sampler in our sprite editor. The color sampler will change the current color
/// based on the color of a pixel on the canvas
///
class Sampler : public Tool
{
    Model &model;
public:
    Sampler (Model &model);
    ///
    /// \brief action This method relays an action over to the model
    /// \param act The action being relayed over to the model
    /// \param x x coordinate sent from the canvas
    /// \param y y coordinate sent from the canvas
    /// \param size The size of the tool, should always be 1 for this tool
    ///
    void action (Action act, int x, int y, int size) override;
};

#endif // SAMPLER_H
