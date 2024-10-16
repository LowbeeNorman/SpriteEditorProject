// Assignment 9
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad
// 4/1/2024
// Reviewed by: Noah Sikorski
#include "sizedialogbox.h"
#include <QDialogButtonBox>
#include <QLayout>
#include <QComboBox>
#include <QLineEdit>

///
/// \brief sizedialogbox::sizedialogbox
/// \param parent The parent QWidget
///
sizedialogbox::sizedialogbox (QWidget *parent) : QDialog (parent)
{
    QVBoxLayout *layout = new QVBoxLayout; //Creates a layout for the dialog box
    this->setLayout (layout);

    comboBox = new QComboBox; //Creates Combobox for selecting the size with 4 options
    QStringList items = QStringList () << "16X16" << "32X32" << "64X64" << "128X128";
    comboBox->addItems (items);

    QDialogButtonBox *buttonBox = new QDialogButtonBox (QDialogButtonBox::Ok); //Create an ok button for exiting dialog box
    connect (buttonBox, SIGNAL (accepted()), this, SLOT (accept ())); //Creates behavior when ok button is clicked

    layout->addWidget (comboBox); //Adds combobox to layout
    layout->addWidget (buttonBox); //Adds ok button to layout
    comboBox->setCurrentIndex (1); //Sets default size to 32X32
}
///
/// \brief sizedialogbox::getCanvasSize
/// \return The size of the canvas chosen by the user
///
int sizedialogbox::getCanvasSize () const
{
    return 16*pow (2,(comboBox->currentIndex ())); //Calculates frame size based off index
}
