#include "canvas.h"
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QWheelEvent>

Canvas::Canvas (QWidget *parent) : QGraphicsView (parent), moving (false) {}

void Canvas::mousePressEvent (QMouseEvent *event) {
    if (event->button () == Qt::MiddleButton) {
        moving = true;
        prevPos = event->pos ();
        event->accept ();
        return;
    }
    QPointF mappedPoint = mapToScene (event->pos ());
    emit mouseClickedOnCanvas (mappedPoint.x (), mappedPoint.y ());
    event->accept ();
}

void Canvas::mouseMoveEvent (QMouseEvent *event) {
    if (moving) {
        QPointF delta = mapToScene (event->pos ()) - mapToScene (prevPos);
        translate (delta.x (), delta.y ());
        prevPos = event->pos ();
        event->accept ();
        return;
    }
    QPointF mappedPoint = mapToScene (event->pos ());
    emit mouseMovedOnCanvas (mappedPoint.x (), mappedPoint.y ());
    event->accept ();
}

void Canvas::mouseReleaseEvent (QMouseEvent *event) {
    if (event->button () == Qt::MiddleButton) {
        moving = false;
        event->accept ();
        return;
    }
    QPointF mappedPoint = mapToScene (event->pos ());
    emit mouseClickedOnCanvas (mappedPoint.x (), mappedPoint.y ());
    event->accept ();
}

bool Canvas::coordinateCheck (QMouseEvent *event) {
    int posX = event->pos ().x ();
    int posY = event->pos ().y ();
    if ((posX > 0 && posX < this->maximumWidth ()) && (posY > 0 && posY < this->maximumHeight ())) {
        return true;
    }
    return false;
}


void Canvas::dragEnterEvent (QDragEnterEvent *event) {
    event->accept ();
}

void Canvas::wheelEvent (QWheelEvent *event) {

    double howMuchWillWeScrollBasedOnAngles = 1.0;
    if (!event->pixelDelta ().isNull ())
        howMuchWillWeScrollBasedOnAngles += event->pixelDelta ().y () / 1200.0;
    else if (!event->angleDelta ().isNull ())
        howMuchWillWeScrollBasedOnAngles += event->angleDelta ().y () / 1200.0;

    QPointF oldMouse = mapToScene (event->position ().toPoint ());

    scale (howMuchWillWeScrollBasedOnAngles, howMuchWillWeScrollBasedOnAngles);

    QPointF newMouse = mapToScene (event->position ().toPoint ());
    QPointF change = newMouse - oldMouse;

    translate (change.x (), change.y ());

    event->accept ();

}

