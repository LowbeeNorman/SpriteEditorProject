///
/// Assignment 9
/// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
/// 4/1/2024
/// Reviewed by: Kyle Stewart
///

#include "filehandler.h"
#include <QJsonDocument>
#include <QByteArray>
#include <QSaveFile>
#include <QFile>
#include <QIODevice>

using namespace filehandler;

bool filehandler::save (const Sprite &sprite, const QString &path) {
    // serialize the sprite
    QJsonDocument doc (sprite.toJson ());
    QByteArray docBytes = doc.toJson ();

    QSaveFile file (path);
    // open file for reading. This method returns false on failure, in this
    // case return false
    if (!file.open (QIODevice::WriteOnly))
        return false;
    // write the data to a temporary file. If this operation fails somehow,
    // return false
    if (file.write (docBytes) < 0)
        return false;
    // write the file to the actual specified path and return the result
    return file.commit ();
}

QJsonDocument filehandler::load (const QString &path) {
    QJsonDocument doc;
    QFile file (path);
    // Return nothing if file cannot be opened
    if (!file.open (QIODevice::ReadOnly))
        return doc;
    // array of bytes contained in file
    QByteArray array = file.readAll ();
    doc = QJsonDocument::fromJson (array);
    return doc;
}


