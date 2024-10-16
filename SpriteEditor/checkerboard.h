///
/// Assignment 9
/// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
/// 4/1/2024
/// Reviewed by: Kyle Stewart
///

#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include <QGraphicsRectItem>
#include <QPainter>

///
/// \brief The Checkerboard class creates a QGraphicsRectItem that will represent the background of
/// our Canvas.
///
class Checkerboard : public QGraphicsRectItem
{
public:
    Checkerboard (const QRectF &rect, QGraphicsItem *parent = nullptr);
    ///
    /// \brief paint Parent paint method that draws to this object. Just fills the rect.
    /// \param painter QPainter that points
    /// \param option Ignore
    /// \param widget Ignore
    ///
    virtual void
    paint (QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
};

#endif // CHECKERBOARD_H
