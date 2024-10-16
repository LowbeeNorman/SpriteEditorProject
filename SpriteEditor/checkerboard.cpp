///
/// Assignment 9
/// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
/// 4/1/2024
/// Reviewed by: Kyle Stewart
///

#include "checkerboard.h"

Checkerboard::Checkerboard (const QRectF &rect, QGraphicsItem *parent)
    : QGraphicsRectItem (rect, parent)
{
    setPen (Qt::NoPen);
    setBrush (QBrush
             (Qt::white,
              QPixmap (QString (":/images/transparent_background.png"))));
}

void Checkerboard::paint
    (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED (option);
    painter->fillRect (rect (), brush ());
}
