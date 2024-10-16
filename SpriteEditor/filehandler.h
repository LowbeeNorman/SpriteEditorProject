///
/// Assignment 9
/// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
/// 4/1/2024
/// Reviewed by: Kyle Stewart
///

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QJsonDocument>
#include "sprite.h"

///
/// This class contains static methods to load and save Sprites using specified paths.
///
namespace filehandler {
///
/// \brief save This method saves a Sprite to the given path in
/// .ssp format.
/// \param sprite Sprite to be saved.
/// \param path Path to save Sprite to.
/// \return true if successful, false otherwise
///
bool save (const Sprite &sprite, const QString &path);

///
/// \brief load This method loads a Sprite from the given path.
/// \param path Path to load Sprite from.
/// \return QJsonDocument containing information about Sprite.
///
QJsonDocument load (const QString &path);

}
#endif // FILEHANDLER_H
