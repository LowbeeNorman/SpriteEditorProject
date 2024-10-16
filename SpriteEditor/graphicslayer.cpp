///
/// Assignment 9
/// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
/// 4/1/2024
/// Reviewed by: Kyle Stewart
///

#include "graphicslayer.h"
#include <QMouseEvent>

GraphicsLayer::GraphicsLayer (QWidget *parent) : QGraphicsView (parent) {}

GraphicsLayer::GraphicsLayer (QGraphicsScene *parent) : QGraphicsView (parent) {
    setScene (parent);
}

void GraphicsLayer::mouseReleaseEvent (QMouseEvent *event) {
    if (coordinateCheck (event))
    {
        emit updateCurrentLayer (this);
    }
}

bool GraphicsLayer::coordinateCheck (QMouseEvent *event) {
    int posX = event->pos ().x ();
    int posY = event->pos ().y ();
    if ((posX > 0 && posX < this->maximumWidth ()) && (posY > 0 && posY < this->maximumHeight ())) {
        return true;
    }
    return false;
}
