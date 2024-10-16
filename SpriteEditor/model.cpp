// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Caleb Norman, Noah Sikorski, and Abdulahad Asim
// 4/1/2024
// Reviewed by: Noah Sikorski

#include "model.h"
#include "filehandler.h"
#include <QJsonDocument>
#include <QImage>
#include "pen.h"
#include "eraser.h"
#include "paintbucket.h"
#include "sampler.h"
#include <QTimer>

Model::Model (QObject *parent)
    : QObject{parent}
    , size (0, 0)
    , millisBetweenFrame (1000)
    , animationFrame (0)
    , sentFrame (0)
    , toolSize (1)
{
    sprite = nullptr;
    currentColor = 0xff440a0a;
    toolbox[static_cast<int> (ToolType::pen)]     = new Pen (*this);
    toolbox[static_cast<int> (ToolType::eraser)]  = new Eraser (*this);
    toolbox[static_cast<int> (ToolType::bucket)]  = new PaintBucket (*this);
    toolbox[static_cast<int> (ToolType::sampler)] = new Sampler (*this);
    currentTool = toolbox[static_cast<int> (ToolType::pen)];
}

Model::~Model () {
    if (nullptr != sprite)
        delete sprite;
    for (unsigned int i = 0; i < sizeof toolbox / sizeof *toolbox; ++i) {
        delete toolbox[i];
    }
}
///
/// \brief Model::setSize
/// \param width new width of canvas
/// \param height new height of canvas
///
void Model::setSize (int width, int height) {
    size.setWidth (width);
    size.setHeight (height);
    sprite = new Sprite (width, height); //Create new sprite
    emit reportFrameAdded (); //Tell view frame and layer have been created
    emit reportLayerAdded ();
}

///
/// \brief Model::setTool
/// \param tool new tool to be set
///
void Model::setTool (ToolType tool) {
    currentTool = toolbox[static_cast<int> (tool)];
}

///
/// \brief Model::setToolSize
/// \param size of tool (Only works for pen & eraser)
///
void Model::setToolSize (int size) {
    toolSize = size;
}

///
/// \brief Model::setActiveFrame
/// \param frame to be set to active
///
void Model::setActiveFrame (size_t frame) {
    if (nullptr == sprite || frame >= sprite->numFrames ()) //If frame is invalid or sprite is null return
        return;
    sprite->setActiveFrame (frame);
    sprite->getActiveFrame ()->setActiveLayer (0); //Set active layer to first layer
    emit reportActiveLayerChanged (0);
    emit sendActiveCanvas (sprite->getActiveFrame ()->getVisibleComposite ()); //Send composite to view
}

///
/// \brief Model::setActiveLayer
/// \param layer to be set to active
///
void Model::setActiveLayer (size_t layer) {
    if (nullptr == sprite) //If sprite is null return
        return;
    if (nullptr == sprite->getActiveFrame ()) //If active frame is invalid return
        return;
    sprite->getActiveFrame ()->setActiveLayer (layer); //Set active layer on active frame
}

///
/// \brief Model::setLayerVisible
/// \param layer to be set to visible
///
void Model::setLayerVisible (size_t layer) {
    if (nullptr == sprite)
        return;
    sprite->setLayerVisible (layer);
    sprite->getActiveFrame ()->invalidate (); //Tell view to update
    emit sendActiveCanvas (sprite->getActiveFrame ()->getVisibleComposite ()); //Get composite to view
}

///
/// \brief Model::addFrame
/// \param index of current frame
///
void Model::addFrame (size_t index) {
    if (nullptr == sprite) //Check for invalid sprite
        return;
    sprite->addFrame (index);
    setActiveFrame (index);
    emit sendActiveCanvas (sprite->getActiveFrame ()->getVisibleComposite ()); //Get new composite to view
    changeMade (); //Tell view to update
}

///
/// \brief Model::addLayer
/// \param index of current layer
///
void Model::addLayer (size_t index) {
    if (nullptr == sprite)//Check for invalid sprite
        return;
    sprite->getActiveFrame ()->insertLayer (index);
    sprite->getActiveFrame ()->setActiveLayer (index);
    emit sendActiveCanvas (sprite->getActiveFrame ()->getVisibleComposite ()); //Get composite to view
    changeMade (); //Tell view to update
}
///
/// \brief Model::deleteActiveLayer
///
void Model::deleteActiveLayer () {
    if (nullptr == sprite) //Check for valid sprite
        return;
    if (nullptr == sprite->getActiveFrame ()) //Check for valid active frame
        return;
    sprite->getActiveFrame ()->deleteActiveLayer ();
    emit sendActiveCanvas (sprite->getActiveFrame ()->getVisibleComposite ()); //Tell view to update
}

///
/// \brief Model::deleteLayer
/// \param index of layer to be deleted
///
void Model::deleteLayer (size_t index) {
    if (nullptr == sprite || nullptr == sprite->getActiveFrame ()) //Check for valid frame/layer
        return;
    sprite->getActiveFrame ()->deleteLayer (index);
    if (0 >= sprite->getActiveFrame ()->getNumLayers ())
        // emit this signal after a delay so the click doesn't carry to the
        // new layer
        QTimer::singleShot (10, this, [this] {emit reportLayerAdded ();});
    QTimer::singleShot (10, this, [this]
                       {
                        emit sendActiveCanvas
                        (sprite->getActiveFrame ()->getVisibleComposite ());
        emit reportActiveLayerChanged (sprite->getActiveFrame ()->getActiveIndex ());
                        });
}

///
/// \brief Model::deleteFrame
///
void Model::deleteFrame () {
    if (nullptr == sprite) //Check for valid sprite
        return;
    sprite->deleteActiveFrame ();
    QTimer::singleShot (10, this, [this]
                       {
                           emit sendActiveCanvas
                               (sprite->getActiveFrame ()->getVisibleComposite ());
                           emit reportActiveLayerChanged (sprite->getActiveFrame ()->getActiveIndex ());
                       });
}

///
/// \brief Model::getLayer
/// \param layer
///
void Model::getLayer (size_t layer) {
    if (nullptr == sprite)
        return;
    QImage *layerGuy = sprite->getActiveFrame ()->getLayer (layer);
    if (nullptr == layerGuy)
        return;
    emit sendLayer (layer, *layerGuy);
}

///
/// \brief Model::load
/// \param path
///
void Model::load (const QString &path) {
    animationFrame = 0;
    sentFrame = 0;
    millisBetweenFrame = 1000;

    QJsonDocument doc = filehandler::load (path);
    sprite = new Sprite (doc);

    size_t frameCount = sprite->getNumFrames ();
    for (size_t i = 0; i < frameCount; i++) {
        emit reportFrameAdded ();
        size_t numLayers = sprite->numLayersFromFrameIndex (i);
        for (size_t j = 0; j < numLayers - 1; j++) {
            emit reportLayerAdded ();
        }
    }

    size = sprite->getSize ();
    emit setCanvasPattern (sprite->getSize ());
}

void Model::save (const QString &path) {
    filehandler::save (*sprite, path);
}

void Model::updateComposite () {
    sprite->getActiveFrame ()->updateComposite ();
}

void Model::getNextPreview () {
    if (nullptr == sprite)
        emit animationFailure ();
    Frame *frameToSend = sprite->getFrame (animationFrame);
    if (nullptr == frameToSend)
        return;
    frameToSend->updateComposite ();
    animationFrame++;
    animationFrame %= sprite->numFrames ();
    QTimer::singleShot
        (millisBetweenFrame, this,
         [this]
          {
            if (sentFrame >= sprite->numFrames ())
                return;
            emit sendActivePreview
                (sprite->getFrame (sentFrame)->getComposite ());
            sentFrame = animationFrame;
          });
}

void Model::getActiveCanvas () {
    if (nullptr == sprite)
        return;
    emit sendActiveCanvas (sprite->getActiveFrame ()->getVisibleComposite ());
}

QRgb Model::getCurrentColor () const {
    return currentColor;
}

void Model::setIndexHue (size_t palletIndex, QRgb hue) {
    palette[palletIndex] = hue;
    currentColor = hue;
}

// used by the sampler to set the color in the palette to the color at (x, y)
void Model::shockAndAwe (int x, int y) {
    if (nullptr == sprite || nullptr == sprite->getActiveFrame ())
        return;
    if (x < 0 || y < 0 || x >= size.width () || y >= size.height ())
        return;
    emit justANormalSignalNothingToSeeHere (getPixel (x, y));
}

void Model::setHue (size_t palletIndex) {
    currentColor = palette[palletIndex];
}

QSize Model::getSize () {
    return size;
}

QRgb Model::getPixel (int x, int y) {
    if (nullptr == sprite || nullptr == sprite->getActiveFrame ())
        return 0x00000000;
    if (x < 0 || y < 0 || x >= size.width () || y >= size.height ())
        return 0x00000000;
    return sprite->getActiveFrame ()->getPixel (x, y);
}

void Model::setPixel (int x, int y) {
    setPixel (x, y, currentColor);
}

void Model::setPixel (int x, int y, QRgb color) {
    if (nullptr == sprite || nullptr == sprite->getActiveFrame ())
        return;
    if (x < 0 || y < 0 || x >= size.width () || y >= size.height ())
        return;
    sprite->getActiveFrame ()->setPixel (x, y, color);
    changeMade ();
}

void Model::changeMade () {
    // no need to check if sprite is null since that should already be done
    sprite->getActiveFrame () -> invalidate ();
    emit activeFrameUpdated ();
    size_t layerIndex = sprite -> getActiveFrame () -> getActiveIndex ();
    emit sendLayer (sprite -> getActiveFrame () -> getActiveIndex (),
                   *sprite -> getActiveFrame () -> getLayer (layerIndex));
}

// mouse slots

void Model::mouseClick   (int x, int y) {
    if (nullptr == currentTool || nullptr == sprite)
        return;
    currentTool->action (Action::start, x, y, toolSize);
}

void Model::mouseMove    (int x, int y) {
    if (nullptr == currentTool || nullptr == sprite)
        return;
    currentTool->action (Action::move, x, y, toolSize);
}

void Model::mouseRelease (int x, int y) {
    if (nullptr == currentTool || nullptr == sprite)
        return;
    currentTool->action (Action::end, x, y, toolSize);
}

///
/// \brief Model::setFPSValue
/// \param fps to be set
///
void Model::setFPSValue (int fps) {
    millisBetweenFrame = 1000 / fps;
}

///
/// \brief Model::importImage
/// \param path of file to be imported
///
void Model::importImage (const QString &path) {
    QImage layer (path);
    sprite->getActiveFrame ()->addLayer (layer);
    sprite->getActiveFrame ()->setActiveLayer (0);
    emit reportLayerAdded ();
    emit sendActiveCanvas (sprite->getActiveFrame ()->getVisibleComposite ());}
