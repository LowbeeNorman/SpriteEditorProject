#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include "sprite.h"
#include "tool.h"

class Model : public QObject
{
    Q_OBJECT
    Sprite *sprite;
    Tool *currentTool;
    QRgb currentColor;
    QRgb palette[16];
    QSize size;

    unsigned int millisBetweenFrame;
    Tool *toolbox[4];
    size_t animationFrame;
    size_t sentFrame;
    int toolSize;

    void changeMade ();
public:
    explicit Model (QObject *parent = nullptr);
    ~Model ();

    void setPixel (int x, int y);
    void setPixel (int x, int y, QRgb color);
    QRgb getPixel (int x, int y);

    QSize getSize ();
    QRgb getCurrentColor () const;

    void shockAndAwe (int x, int y);
signals:
    void sendActivePreview (const QPixmap &preview);
    void sendActiveCanvas  (const QPixmap &canvas);
    void sendLayer (size_t layerIndex, const QImage &layer);
    void activeFrameUpdated ();
    void justANormalSignalNothingToSeeHere (QRgb hue);
    void animationFailure ();

    void setCanvasPattern (QSize imageSize);

    void reportFrameAdded ();
    void reportLayerAdded ();
    void reportActiveLayerChanged (size_t layer);

public slots:
    void load (const QString &path);
    void save (const QString &path);
    void importImage (const QString &path);
    void updateComposite ();
    void getNextPreview ();
    void getActiveCanvas ();

    void setTool (ToolType tool);
    void setToolSize (int size);
    void setSize (int width, int height);
    void setActiveFrame (size_t frame);
    void setActiveLayer (size_t layer);
    void setLayerVisible (size_t layer);
    // acts on the active frame
    void getLayer (size_t layer);
    // will set the current color as well as the color in the array
    void setIndexHue (size_t palletIndex, QRgb hue);
    void setHue (size_t palletIndex);

    void addFrame (size_t index);
    void addLayer (size_t index);

    // Assuming these slots will be used to delete the active layer/frame
    void deleteActiveLayer ();
    void deleteLayer (size_t index);
    void deleteFrame ();

    void mouseClick   (int x, int y);
    void mouseMove    (int x, int y);
    void mouseRelease (int x, int y);

    void setFPSValue (int fps);
};

#endif // MODEL_H
