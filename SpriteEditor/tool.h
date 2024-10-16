// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Caleb Norman
#ifndef TOOL_H
#define TOOL_H

#include <QDebug>

///
/// \brief This class enum provides an abstraction for different tool actions
///
enum class Action {
    start, move, end
};
///
/// \brief This class enum provides an abstraction for different tool types
///
enum class ToolType {
    pen, eraser, bucket, sampler
};

///
/// \brief The tool class is a base class that provides an action method for the tool to implement
///
class Tool {
public:
    ///
    /// \brief action This method is an action that each derived class will implement
    /// \param act The action from the derived class
    /// \param x x coordinate given from the canvas
    /// \param y y coordinate given from the canvas
    /// \param size the size of the tool
    ///
    virtual void action (Action act, int x, int y, int size) = 0;
};

#endif // TOOL_H
