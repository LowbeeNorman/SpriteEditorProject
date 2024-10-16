// Assignment 9
// names Jayden Ferrin, Abdulahad Asim, Winston Ji
// 4/1/2024
// Reviewed by: Jayden Ferrin

#ifndef SPRITE_H
#define SPRITE_H

#include "frame.h"
#include <QList>
#include <QJsonObject>
#include <QJsonDocument>

///
/// \brief The Sprite class defines a sprite used for drawing, saving, etc.
/// Holds every frame and provides methods to work with sprites
///
class Sprite {
    QList<Frame> frames;
    Frame *activeFrame;
    size_t activeFrameIndex;
    QSize size;
public:
    ///
    /// \brief Sprite Constructor to create a sprite object with the specified
    ///  width and height
    /// \param width The width of the sprite
    /// \param height The height of the sprite
    ///
    Sprite (int width, int height);
    ///
    /// \brief Sprite constructor used to load a sprite from json
    /// \param doc the json document holding the sprite
    ///
    Sprite (QJsonDocument &doc);
    ///
    /// \brief toJson used to save the sprite to json
    /// \return A json object to be written to a file representing this sprite
    ///
    QJsonObject toJson () const;

    ///
    /// \brief getActiveFrame gets the frame being actively viewed and edited
    /// by the user
    /// \return the active frame
    ///
    Frame *getActiveFrame ();
    ///
    /// \brief getFrame gets the frame in the sprite
    /// \param frame the index of the frame to return
    /// \return a pointer to the frame at the specified index
    ///
    Frame *getFrame (size_t frame);
    ///
    /// \brief deleteActiveFrame deletes the active frame from the sprite
    /// and sets the active frame to the first one
    ///
    void deleteActiveFrame ();
    ///
    /// \brief addFrame adds the specified frame to the sprite
    /// \param f the frame to add
    ///
    void addFrame (Frame &f);
    ///
    /// \brief addFrame adds an empty frame to the sprite
    ///
    void addFrame ();
    ///
    /// \brief addFrame inserts a frame into the sprite at the specified index
    /// \param index the index to insert the frame to
    ///
    void addFrame (size_t index);
    ///
    /// \brief numFrames returns the number of frames in the sprite
    /// \return the number of frames in the sprite
    ///
    size_t numFrames ();
    ///
    /// \brief numLayersFromFrameIndex returns the number of layers held by
    /// the frame at the specified index
    /// \param index index in the sprite of the frame to check
    /// \return the number of layers in the specified frame
    ///
    size_t numLayersFromFrameIndex (size_t index);
    size_t getNumFrames ();

    ///
    /// \brief setActiveFrame sets the active frame to the one at the specified
    /// index
    /// \param index index of the frame to be set to active
    ///
    void setActiveFrame (size_t index);
    ///
    /// \brief setLayerVisible changes the visibility of the frame at the
    /// specified index to the opposite of its current visibility
    /// \param index the index of the layer
    ///
    void setLayerVisible (size_t index);

    ///
    /// \brief getSize gets the size of the sprite
    /// \return the size of the sprite
    ///
    QSize getSize () const;


};


#endif // SPRITE_H
