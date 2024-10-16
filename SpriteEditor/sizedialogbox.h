// Assignment 9
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad
// 4/1/2024
// Reviewed by: Noah Sikorski
#ifndef SIZEDIALOGBOX_H
#define SIZEDIALOGBOX_H
#include <QDialog>
#include <QComboBox>

///
/// \brief The sizedialogbox class prompts the user for a canvas size with a drop down box
///
class sizedialogbox : public QDialog
{
    Q_OBJECT

public:
    explicit sizedialogbox (QWidget *parent = 0);
    int getCanvasSize () const;

private:
    QComboBox *comboBox;
};

#endif // SIZEDIALOGBOX_H
