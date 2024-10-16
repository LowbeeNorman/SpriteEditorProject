#include "canvas_old.h"
#include <QMouseEvent>
#include <QDragEnterEvent>

Canvas_old::Canvas_old (QWidget* parent) : QGraphicsView (parent) {}

void Canvas_old::mousePressEvent (QMouseEvent *event) {

    emit mouseClickedOnCanvas (event->pos ().x (), event->pos ().y ());
    // qInfo () << "Clicked: " <<event->pos ();

}

void Canvas_old::mouseMoveEvent (QMouseEvent *event) {
    if (coordinateCheck (event))
    {
        emit mouseMovedOnCanvas (event->pos ().x (), event->pos ().y ());
        // qInfo () << "Move: " <<event->pos ();
    }
}

void Canvas_old::mouseReleaseEvent (QMouseEvent *event) {
    if (coordinateCheck (event))
    {
        emit mouseReleasedOnCanvas (event->pos ().x (), event->pos ().y ());
        // qInfo () << "Released: " <<event->pos ();
        //delete this;
    }

}

bool Canvas_old::coordinateCheck (QMouseEvent *event) {
    int posX = event->pos ().x ();
    int posY = event->pos ().y ();
    if ((posX > 0 && posX < this->maximumWidth ()) && (posY > 0 && posY < this->maximumHeight ())) {
        return true;
    }
    return false;
}


void Canvas_old::dragEnterEvent (QDragEnterEvent *event) {
    qInfo () << event->position ();
    qInfo () << event->source ()->objectName ();
}
