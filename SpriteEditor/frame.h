// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Abdulahad Asim
// 4/1/2024
// Reviewed by: Jayden Ferrin

#ifndef FRAME_H
#define FRAME_H

#include <QList>
#include <QImage>
#include <QPixmap>
#include <QJsonObject>

class Frame {
    // the layers in the frame
    QList<QImage> layers;
    // the visibility of each frame
    QList<bool> visibility;
    // size of the frame
    QSize size;

    // composite for displaying the preview
    QPixmap composite;
    // whether the composite should be redrawn (false if it should)
    bool compositeValid;
    // composite image for displaying the canvas
    QPixmap visibleComposite;
    // the layer being actively edited and viewed by the user
    QImage *activeLayer;
    // whether the visible composite should be redrawn
    bool visibleCompositeValid;
    // the index of the active layer
    size_t activeLayerIndex;

public:
    Frame (QSize size);
    Frame (QJsonObject obj);
    ///
    /// \brief toJson used to save this frame to json
    /// \return a json object representing the frame
    ///
    QJsonObject toJson () const;

    ///
    /// \brief updateComposite recreates the composite image to reflect
    /// changes that have been made in the frame
    ///
    void updateComposite ();
    ///
    /// \brief getComposite gets the composite image for the preview
    /// \return a reference to the composite image
    ///
    QPixmap &getComposite ();
    ///
    /// \brief getVisibleComposite gets the currently visible composite for
    /// the canvas consisting of a combination of every visible layer
    /// \return a reference to the visible composite image
    ///
    QPixmap &getVisibleComposite ();
    ///
    /// \brief invalidate notifies the frame that its composite images must
    /// be recreated since a change has been made to at least one layer
    ///
    void invalidate ();

    ///
    /// \brief addLayer adds the specified layer to the frame
    /// \param layer the layer to add
    ///
    void addLayer (QImage &layer);
    ///
    /// \brief addLayer adds an empty layer to the frame
    ///
    void addLayer ();
    ///
    /// \brief insertLayer inserts an empty layer to the frame at the specified
    /// index
    /// \param index position in the frame to insert the layer
    ///
    void insertLayer (size_t index);
    ///
    /// \brief addLayer adds the specified image to the frame at the specified
    /// layer index
    /// \param layer image to be added. Shrinks the image to be the same size
    /// as the frame
    /// \param index index of the layer to add the image in
    /// \return the index the layer was added to, since it can be different
    ///
    size_t addLayer (QImage &layer, size_t index);
    ///
    /// \brief deleteActiveLayer deletes the active layer and changes the
    /// active layer to the next valid layer
    ///
    void deleteActiveLayer ();
    ///
    /// \brief deleteLayer deletes the layer at the specified index
    /// \param index index to delete the layer at
    ///
    void deleteLayer (size_t index);
    ///
    /// \brief getLayer gets the layer at the specified index
    /// \param layer the index of the layer
    /// \return a pointer to the image of the specified layer
    ///
    QImage *getLayer (size_t layer);
    ///
    /// \brief getPixel gets the color of the active layer at (x, y)
    /// \param x the x coordinate of the pixel
    /// \param y the x coordinate of the pixel
    /// \return a Qrgb color for the color of the layer at the coordinates
    ///
    QRgb getPixel (int x, int y);

    void setVisible (size_t index);
    bool getVisible (size_t index);

    void setActiveLayer (size_t layer);
    size_t getActiveIndex ();
    void setPixel (int x, int y, QRgb color);

    QSize getSize ();

    size_t getNumLayers () const;
private:
    void updateComposite (QPixmap &composite);
};


#endif // FRAME_H
