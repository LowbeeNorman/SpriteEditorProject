///
/// Assignment 9
/// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
/// 4/1/2024
/// Reviewed by: Kyle Stewart
///

#ifndef GRAPHICSLAYER_H
#define GRAPHICSLAYER_H

#include <QGraphicsView>
///
/// \brief The GraphicsLayer class creates a QGraphicsLayer that will contain the pixmap
/// for each layer.
///
class GraphicsLayer : public QGraphicsView
{
    Q_OBJECT

    ///
    /// \brief coordinateCheck Checks if the given event is at a
    /// valid position.
    /// \param event The Event that was conducted.
    /// \return true if valid coordinate, false otherwise
    ///
    bool coordinateCheck (QMouseEvent *event);
public:
    GraphicsLayer (QWidget *parent = nullptr);
    GraphicsLayer (QGraphicsScene *parent = nullptr);

protected:
    ///
    /// \brief mouseReleaseEvent Informs the view that the layer has
    /// has been updated.
    /// \param event The Event that was conducted.
    ///
    void mouseReleaseEvent (QMouseEvent *event);

signals:
    ///
    /// \brief updateCurrentLayer Informs the view that the current layer
    /// contained in the view has been updated.
    /// \param view View that contains the layer.
    ///
    void updateCurrentLayer (QGraphicsView *view);
};

#endif // GRAPHICSLAYER_H
