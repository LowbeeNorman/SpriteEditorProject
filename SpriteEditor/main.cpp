#include "mainview.h"

#include <QApplication>

int main (int argc, char *argv[])
{
    QApplication a (argc, argv);
    Model m;
    MainView w (m);
    w.show ();
    return a.exec ();
}
