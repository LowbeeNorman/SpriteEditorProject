// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#ifndef PEN_H
#define PEN_H

#include "tool.h"
#include "model.h"

///
/// \brief This class represents our pen tool in our sprite editor, which can modify pixels in a specified range
///
class Pen : public Tool
{
    Model &model;
public:
    Pen (Model &model);
    ///
    /// \brief action This method relays the drawing action over to the model
    /// \param act The pen action
    /// \param x x coordinate sent from the canvas
    /// \param y y coordinate sent from the canvas
    /// \param size The size of the tool
    ///
    void action (Action act, int x, int y, int size) override;
};

#endif // PEN_H
