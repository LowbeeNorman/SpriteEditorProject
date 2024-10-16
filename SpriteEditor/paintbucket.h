// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#ifndef PAINTBUCKET_H
#define PAINTBUCKET_H

#include "tool.h"
#include "model.h"

///
/// \brief This class represents the paint bucket, which fills a region of connected, similar pixels with the selected color
///
class PaintBucket : public Tool  {
    Model &model;
    ///
    /// \brief This method is an algorithm to calculate all connected pixels that need to be filled
    /// \param x x coordinate from canvas
    /// \param y y coordinate from canvas
    /// \param targetColor The color to change flooded pixels to
    ///
    void floodFill (int x, int y, QRgb targetColor);
public:
    PaintBucket (Model &model);
    ///
    /// \brief action This method relays an action over to the model
    /// \param act The relayed action, which will be sent to the model
    /// \param x x coordinate from the canvas
    /// \param y y coordinate from the canvas
    /// \param size The size of the tool, always will be 1
    ///
    void action (Action act, int x, int y, int size) override;

};

#endif // PAINTBUCKET_H
