#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QPixmap>
#include "model.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QScrollArea>
#include <QPushButton>
#include <QHash>
#include <chrono>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainView;
}
QT_END_NAMESPACE

class MainView : public QMainWindow
{
    Q_OBJECT

public:
    MainView (Model &model, QWidget *parent = nullptr);
    ~MainView ();

private:
    Ui::MainView *ui;
    // item used to display the preview image
    QGraphicsPixmapItem *previewItem;
    QGraphicsPixmapItem *canvasItem;
    // scene to be used as the scene of the preview graphics view
    QGraphicsScene *previewScene;
    QGraphicsScene *canvasScene;
    QSize imageSize;

    int frameIndex = 0;
    int layerIndex = -1;
    int count = 1;

    bool removingLayers = false;
    QPushButton *colorButtons[16];
    size_t whichHueButtonIsPressed;
    bool animating;

    void thisMethodIsNotSuspicious (QColor hue);
    void insertLayer (QPoint &frameLayer);
    void insertFrame (int frame);
    void moveLayersDelete (QPoint &frameLayer);
    void moveFramesDelete (size_t frame);
    QHash<QPoint, QGraphicsView*>  layersFromFrameLayer;
    QHash<QGraphicsView*, QPoint> frameLayerFromLayers;
    QList<size_t> layerCounts;

    std::chrono::time_point<std::chrono::system_clock> lastSeenFrame;

signals:
    void requestPreview ();
    void requestCanvas ();
    void goPreviousFrame (int currentTabIndex);
    void goNextFrame (int currentTabIndex);
    void setActiveFrame (size_t frameIndex);
    void setActiveLayer (size_t layerIndex);
    void addFrame (size_t currentTabIndex);
    void deleteFrame ();
    void deleteLayer (size_t index);
    void addLayer (size_t index);
    void changeLayerVisibility (size_t layer);
    void colorChanged (QColor color);

    void requestLayer (size_t layerIndex);

    void setTool (ToolType tool);
    void sendToolSize (int size);
    void setHue (size_t palletIndex);
    void setIndexHue (size_t palletIndex, QRgb hue);

    void saveSelected (QString& path);
    void openSelected (QString& path);
    void importImageSelected (QString& path);
    void setLayerVisibility (size_t layer);

public slots:
    void receivePreview (const QPixmap &preview);
    void receiveCanvas (const QPixmap &canvas);

    void processUpdate ();
    void acknowledgeAnimationFailure ();
    void startAnimating (int value);

    void receiveLayer (size_t layerIndex, const QImage &layer);

    void nextFrameClicked ();
    void previousFrameClicked ();
    void newFrameClicked ();
    void guiNewFrame ();
    void deleteFrameClicked ();
    void newLayerClicked ();
    void guiNewLayer ();
    void deleteLayerClicked ();
    void colorPickerClicked ();

    void toolSizeChanged1 ();
    void toolSizeChanged3 ();
    void toolSizeChanged5 ();
    void toolSizeChanged7 ();

    void nothingOutOfTheOrdinary (QRgb hue);

    void tabIndexChanged (int tabIndex);
    void receiveUpdateCurrentLayer (QGraphicsView *view);
    void changeActiveLayer (size_t index);
    void removeLayerFromScrollArea (QScrollArea* scrollArea, QGraphicsView* newViewLayer);
    void determineVisibility (QGraphicsView* view, QScrollArea* scrollArea, QPushButton* visibilityButton);

    void saveMenuClicked ();
    void openMenuClicked ();
    void importImageMenuClicked ();
    void updateCanvasPattern (QSize imageSize);
    void helpActionTriggered (QAction* action);

private slots:
    // auto generated slots
    void on_penButton_clicked (bool checked);
    void on_eraserButton_clicked (bool checked);
    void on_paintBucketButton_clicked (bool checked);
    void on_colorSamplerButton_clicked (bool checked);
    void on_red_clicked (bool checked);
    void on_redOrange_clicked (bool checked);
    void on_orange_clicked (bool checked);
    void on_yellowOrange_clicked (bool checked);
    void on_yellow_clicked (bool checked);
    void on_yellowGreen_clicked (bool checked);
    void on_green_clicked (bool checked);
    void on_blueGreen_clicked (bool checked);
    void on_blue_clicked (bool checked);
    void on_bluePurple_clicked (bool checked);
    void on_purple_clicked (bool checked);
    void on_pink_clicked (bool checked);
    void on_white_clicked (bool checked);
    void on_lightGray_clicked (bool checked);
    void on_darkGray_clicked (bool checked);
    void on_black_clicked (bool checked);
};
#endif // MAINVIEW_H

