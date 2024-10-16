// Assignment 9: Sprite Editor
// Kyle Stewart, Winston Ji, Jayden Ferrin, Abdulahad Asim
// 4/1/2024
// Reviewed by: Jayden Ferrin

#include "frame.h"

#include <QBuffer>
#include <QJsonArray>
#include <QPainter>

Frame::Frame (QSize size)
    : activeLayer (nullptr)
    , compositeValid (false)
    , visibleCompositeValid (false)
    , size (size)
    , composite (size)
    , visibleComposite (size)
{}

Frame::Frame (QJsonObject obj)
    : compositeValid (false)
    , activeLayer (nullptr)
    , visibleCompositeValid (false)
    , size (16, 16)
    , composite (QSize (16, 16))
    , visibleComposite (QSize (16, 16))
{

    QJsonArray arr = obj["layers"].toArray ();
    for (const QJsonValue &value : arr) {
        QImage layer;
        QJsonObject layerObject;
        if (value.isObject ())
            layerObject = value.toObject ();
        QByteArray base64Arr;
        if (layerObject["image"].isString ()) {
            base64Arr = layerObject["image"].toString ().toUtf8 ();
        }
        QByteArray image = QByteArray::fromBase64 (base64Arr);
        layer.loadFromData (image);
        layers.append (layer);
        visibility.append (true);
        if (size.height () == 0)
            size = layer.size ();

        // set the size of the Sprite to be whatever was passed through the
        // image loaded.
        size = layer.size ();
        composite = QPixmap (size);
        visibleComposite = QPixmap (size);
    }
    updateComposite ();
    if (nullptr == activeLayer)
        setActiveLayer (0);
}

void Frame::addLayer () {
    insertLayer (layers.size ());
}

void Frame::insertLayer (size_t index) {
    QImage layer (size, QImage::Format_ARGB32);
    index = addLayer (layer , index);
    layers[index].fill (Qt::transparent);
}

void Frame::addLayer (QImage &layer) {
    QImage scaled = layer.scaled (size, Qt::KeepAspectRatio);
    addLayer (scaled, layers.size ());
}

size_t Frame::addLayer (QImage &layer, size_t index) {
    if (index > (unsigned) layers.size ())
        index = layers.size ();
    layers.insert (index, layer);
    visibility.insert (index, true);
    invalidate ();
    updateComposite ();
    if (nullptr == activeLayer)
        setActiveLayer (0);
    return index;
}

void Frame::invalidate () {
    compositeValid = false;
    visibleCompositeValid = false;
}

void Frame::updateComposite () {
    if (nullptr == activeLayer || compositeValid) {
        return;
    }
    composite.fill (Qt::transparent);
    QPainter painter (&composite);
    painter.setBackgroundMode (Qt::TransparentMode);
    painter.setCompositionMode (QPainter::CompositionMode_SourceOver);
    for (const auto &layer : layers) {
        painter.drawImage (QPoint (0, 0), layer);
    }
    compositeValid = true;
}

QPixmap &Frame::getComposite () {
    if (composite.isNull ()) {
        updateComposite ();
    }
    return composite;
}

QPixmap &Frame::getVisibleComposite () {
    if (visibleCompositeValid)
        return visibleComposite;
    bool allVisible = true;
    for (const bool &visible : visibility) {
        allVisible &= visible;
    }
    if (allVisible) {
        updateComposite ();
        return getComposite ();
    }
    visibleComposite.fill (Qt::transparent);
    QPainter painter (&visibleComposite);
    painter.setBackgroundMode (Qt::TransparentMode);
    painter.setCompositionMode (QPainter::CompositionMode_SourceOver);
    for (unsigned int i = 0; i < layers.size (); ++i) {
        if (visibility[i])
            painter.drawImage (QPoint (0, 0), layers[i]);
    }
    visibleCompositeValid = true;
    return visibleComposite;
}

QImage *Frame::getLayer (size_t layer) {
    if (layer >= layers.size ())
        return nullptr;
    return &layers[layer];
}

size_t Frame::getActiveIndex () {
    return activeLayerIndex;
}

void Frame::setActiveLayer (size_t layer) {
    if (layer >= layers.size ())
        return;
    activeLayer = &layers[layer];
    activeLayerIndex = layer;
}

void Frame::deleteActiveLayer () {
    if (nullptr == activeLayer)
        return;
    layers.remove (activeLayerIndex);
    activeLayer = &layers[0];
    activeLayerIndex = 0;
    invalidate ();
}

void Frame::deleteLayer (size_t index) {
    if (index >= layers.size ())
        return;
    layers.remove (index);
    if (0 == layers.size ())
        addLayer ();
    if (0 == index && activeLayerIndex == index)
        setActiveLayer (0);
    else if (activeLayerIndex >= index)
        setActiveLayer (activeLayerIndex - 1);
    invalidate ();
}

QRgb Frame::getPixel (int x, int y) {
    return activeLayer->pixel (x, y);
}

void Frame::setPixel (int x, int y, QRgb color) {
    activeLayer->setPixel (x, y, color);
}

void Frame::setVisible (size_t index) {
    if (index >= visibility.size ())
        return;
    invalidate ();
    visibility[index] = !visibility[index];
}

bool Frame::getVisible (size_t index) {
    if (index >= visibility.size ())
        return false;
    return visibility[index];
}

QJsonObject Frame::toJson () const {
    QJsonObject completeObj;
    QJsonArray imageArr;
    for (auto &image : this->layers) {
        qInfo () << image;
        QJsonObject currentObj;

        // Convert QImage to base64-encoded string
        QByteArray byteArray;
        QBuffer buffer (&byteArray);
        buffer.open (QIODevice::WriteOnly);
        image.save (&buffer, "PNG");
        QString base64Image = QString::fromLatin1 (byteArray.toBase64 ().data ());

        // Add base64-encoded image to JSON object
        currentObj["image"] = base64Image;
        imageArr.append (currentObj);
    }
    completeObj["layers"] = imageArr;

    return completeObj;
}

QSize Frame::getSize () {
    return size;
}

size_t Frame::getNumLayers () const {
    return layers.size ();
}
