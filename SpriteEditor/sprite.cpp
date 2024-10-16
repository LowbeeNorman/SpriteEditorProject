// Assignment 9
// names Jayden Ferrin, Abdulahad Asim, Winston Ji
// 4/1/2024
// Reviewed by: Jayden Ferrin

#include "sprite.h"
#include <QJsonArray>
#include <QJsonObject>

Sprite::Sprite (int width, int height)
    : size (width, height)
    , activeFrame (nullptr)
{
    addFrame ();
}

Sprite::Sprite (QJsonDocument &doc) {
    QJsonObject obj = doc.object ();
    QJsonArray arr = obj["frames"].toArray ();
    for (const auto & value : arr) {
        QJsonObject frame = value.toObject ();
        frames.append (Frame (frame));
    }
    setActiveFrame (0);
    size = activeFrame->getSize ();
}

void Sprite::addFrame () {
    Frame f (size);
    f.addLayer ();
    addFrame (f);
}

void Sprite::addFrame (Frame &f) {
    frames.append (f);
    if (nullptr == activeFrame)
        setActiveFrame (0);
}

void Sprite::addFrame (size_t index) {
    if (index > frames.size ())
        index = frames.size ();
    Frame f (size);
    f.addLayer ();
    frames.insert (index, f);
    if (nullptr == activeFrame)
        setActiveFrame (0);
}

QJsonObject Sprite::toJson () const {
    QJsonObject obj;
    QJsonArray arr;
    for (auto& frame : this->frames) {
        arr.append (frame.toJson ());
    }
    obj["frames"] = arr;
    return obj;
}

Frame *Sprite::getActiveFrame () {
    return activeFrame;
}

Frame *Sprite::getFrame (size_t frame) {
    if (frame >= frames.size ())
        return nullptr;
    return &frames[frame];
}

size_t Sprite::numFrames () {
    return frames.size ();
}

void Sprite::deleteActiveFrame () {
    frames.remove (activeFrameIndex);
    setActiveFrame (0);
}

void Sprite::setActiveFrame (size_t index) {
    if (index >= frames.size ())
        index = frames.size () - 1;
    activeFrame = &frames[index];
    activeFrameIndex = index;
}

void Sprite::setLayerVisible (size_t index) {
    activeFrame->setVisible (index);
}

QSize Sprite::getSize () const {
    return size;
}

size_t Sprite::getNumFrames () {
    return frames.size ();
}

size_t Sprite::numLayersFromFrameIndex (size_t index) {
    return frames.at (index).getNumLayers ();
}


