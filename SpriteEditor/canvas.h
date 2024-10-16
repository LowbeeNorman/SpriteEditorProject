///
/// This class contains code to represent the sprite editor canvas
///
/// Assignment 9
/// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
/// 4/1/2024
/// Reviewed by: Winston Ji
///

#ifndef CANVAS_H
#define CANVAS_H

#include <QGraphicsView>
#include <QObject>

class Canvas : public QGraphicsView
{
    Q_OBJECT

    bool coordinateCheck (QMouseEvent *event);
    bool moving;
    QPoint prevPos;
public:
    Canvas (QWidget *parent = nullptr);

protected:
    // Overridden mouse even methods for drawing on canvas.
    // Tools follow a start, middle, stop event framework using these methods.
    void mousePressEvent (QMouseEvent *event) override;
    void mouseMoveEvent (QMouseEvent *event) override;
    void mouseReleaseEvent (QMouseEvent *event) override;
    void dragEnterEvent (QDragEnterEvent *event) override;
    void wheelEvent (QWheelEvent *event) override;

signals:
    // Signals for communicating with MainView
    void mouseClickedOnCanvas (int x, int y);
    void mouseMovedOnCanvas (int x, int y);
    void mouseReleasedOnCanvas (int x, int y);
};

#endif // CANVAS_H
