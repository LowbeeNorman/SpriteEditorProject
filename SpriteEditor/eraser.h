// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#ifndef ERASER_H
#define ERASER_H

#include "tool.h"
#include "model.h"

///
/// \brief This class is for our eraser, which inherits from tool. The eraser will reset pixels on a canvas.
///
class Eraser : public Tool {
    Model &model; // The reference for the model
public:
    Eraser (Model &model);
    ///
    /// \brief action This method holds the action of a tool
    /// \param act The action that
    /// \param x x coordinate sent by the canvas
    /// \param y y coordinate sent by the canvas
    /// \param size the size of the tool
    ///
    void action (Action act, int x, int y, int size) override;
};

#endif // ERASER_H
