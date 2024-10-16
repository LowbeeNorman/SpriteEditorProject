#ifndef CANVAS_OLD_H
#define CANVAS_OLD_H

#include <QGraphicsView>

class Canvas_old : public QGraphicsView
{
    bool coordinateCheck (QMouseEvent *event);
public:
    Canvas_old (QWidget* parent = nullptr);


protected:
    void mousePressEvent (QMouseEvent *event) override;
    void mouseMoveEvent (QMouseEvent *event) override;
    void mouseReleaseEvent (QMouseEvent *event) override;
    void dragEnterEvent (QDragEnterEvent *event) override;


signals:
    void mouseClickedOnCanvas (int x, int y);
    void mouseMovedOnCanvas (int x, int y);
    void mouseReleasedOnCanvas (int x, int y);

};

#endif // CANVAS_OLD_H
