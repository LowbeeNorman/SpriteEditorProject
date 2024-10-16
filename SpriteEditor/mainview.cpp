#include "mainview.h"
#include "ui_mainview.h"
#include "sizedialogbox.h"
#include "canvas.h"
#include <QGraphicsScene>
#include <QBrush>
#include <QPixmap>
#include <QScrollBar>
#include <QTimer>
#include "graphicslayer.h"
#include <QIcon>
#include <QColorDialog>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QTimer>

#include "checkerboard.h"

MainView::MainView (Model &model, QWidget *parent)
    : QMainWindow (parent)
    , ui (new Ui::MainView)
    , whichHueButtonIsPressed (15)
    , animating (false)

{
    sizedialogbox sizedialogbox;
    if (sizedialogbox.exec ())
    {
        //use sizedialogbox.getCanvasSize to change size of canvas
        imageSize = QSize (sizedialogbox.getCanvasSize (), sizedialogbox.getCanvasSize ());
        model.setSize (imageSize.width (), imageSize.height ());
    }
    else
    {
        // qInfo () << "nothing selected";
        exit (EXIT_SUCCESS);
    }

    ui->setupUi (this);

    MainView::canvasScene = new QGraphicsScene (ui->mainCanvas);
    MainView::previewScene = new QGraphicsScene (ui->Preview);
    // ui->mainCanvas->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    // ui->mainCanvas->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    ui->Preview->setHorizontalScrollBarPolicy (Qt::ScrollBarAlwaysOff);
    ui->Preview->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOff);

    connect (this, &MainView::requestPreview, &model, &Model::getNextPreview);
    connect (this, &MainView::requestCanvas,  &model, &Model::getActiveCanvas);
    connect (this, &MainView::requestLayer,   &model, &Model::getLayer);
    connect (&model, &Model::sendActivePreview, this, &MainView::receivePreview);
    connect (&model, &Model::sendActiveCanvas,  this, &MainView::receiveCanvas);
    connect (&model, &Model::sendLayer,         this, &MainView::receiveLayer);
    connect (&model, &Model::animationFailure, this, &MainView::acknowledgeAnimationFailure);


    previewScene = new QGraphicsScene ();
    previewItem = new QGraphicsPixmapItem ();
    canvasScene = new QGraphicsScene ();
    canvasItem = new QGraphicsPixmapItem ();
    // set the background brush of the canvas scene to dark gray
    canvasScene->setBackgroundBrush (QBrush (Qt::darkGray));
    previewScene->setBackgroundBrush (QBrush (Qt::darkGray));
    // set the background of the image to the checkerboard to show what parts
    // are transparent
    Checkerboard *checkers = new Checkerboard (QRectF (QPointF (0, 0), imageSize));
    canvasScene->addItem (checkers);
    // add the preview item to the scene so it can be changed dynamically
    previewScene->addItem (previewItem);
    canvasScene->addItem (canvasItem);
    // set the scene of the preview  and canvas to the one we set up
    ui->Preview->setScene (previewScene);
    ui->mainCanvas->setScene (canvasScene);
    QRectF canvasRect = ui->mainCanvas->contentsRect ();
    // scale the graphics view so the sprite fits real nice in the screen
    ui->mainCanvas->scale
        ((canvasRect.width () * 1.0) / imageSize.width (),
         (canvasRect.width () * 1.0) / imageSize.width ());
    ui->mainCanvas->setTransformationAnchor (QGraphicsView::NoAnchor);
    // ui->mainCanvas->setResizeAnchor (QGraphicsView::AnchorUnderMouse);


    ui->frameTabs->removeTab (0);
    ui->frameTabs->removeTab (0);
    QScrollArea* firstFrame = new QScrollArea ();
    ui->frameTabs->addTab (firstFrame, QString ("Frame 1"));

    //connect (ui->previousFrameButton, &QPushButton::clicked, this, &MainView::previousFrameClicked);
    //connect (this, &MainView::goPreviousFrame, ui->frameTabs, &QTabWidget::setCurrentIndex);

    connect (ui->nextFrameButton, &QPushButton::clicked, this, &MainView::nextFrameClicked);
    connect (ui->previousFrameButton, &QPushButton::clicked, this, &MainView::previousFrameClicked);
    //connect (this, &MainView::goNextFrame, ui->frameTabs, &QTabWidget::setCurrentIndex);

    connect (ui->newFrameButton, &QPushButton::clicked, this, &MainView::newFrameClicked);
    connect (ui->deleteFrameButton, &QPushButton::clicked, this, &MainView::deleteFrameClicked);
    connect (ui->newLayerButton, &QPushButton::clicked, this, &MainView::newLayerClicked);
    connect (ui->deleteLayerButton, &QPushButton::clicked, this, &MainView::deleteLayerClicked);
    //connect (this, &MainView::addFrame, ui->frameTabs, &QTabWidget::addTab);

    // connect signals for adding frames, layers, etc.
    connect (this, &MainView::addFrame, &model, &Model::addFrame);
    connect (this, &MainView::addLayer, &model, &Model::addLayer);
    connect (this, &MainView::deleteLayer, &model, &Model::deleteLayer);
    connect (this, &MainView::setActiveFrame, &model, &Model::setActiveFrame);
    connect (this, &MainView::deleteFrame, &model, &Model::deleteFrame);
    connect (this, &MainView::setActiveLayer, &model, &Model::setActiveLayer);
    connect (ui->frameTabs, &QTabWidget::currentChanged, this, &MainView::tabIndexChanged);
    connect (this, &MainView::setLayerVisibility, &model, &Model::setLayerVisible);
    connect (&model, &Model::reportFrameAdded, this, &MainView::guiNewFrame);
    connect (&model, &Model::reportLayerAdded, this, &MainView::guiNewLayer);
    connect (&model, &Model::reportActiveLayerChanged, this, &MainView::changeActiveLayer);

    connect (ui->menuHelp, &QMenu::triggered, this, &MainView::helpActionTriggered);

    // connect signals from the canvas for mouse events to slots in the model
    connect (ui->mainCanvas, &Canvas::mouseClickedOnCanvas,   &model, &Model::mouseClick);
    connect (ui->mainCanvas, &Canvas::mouseMovedOnCanvas,     &model, &Model::mouseMove);
    connect (ui->mainCanvas, &Canvas::mouseReleasedOnCanvas,  &model, &Model::mouseRelease);
    // connect signals to make drawing work
    // connect (&model, &Model::activeFrameUpdated, &model, &Model::updateComposite);
    connect (&model, &Model::activeFrameUpdated, this, &MainView::processUpdate);

    connect (this, &MainView::changeLayerVisibility, &model, &Model::setLayerVisible);

    // connection for fps slider
    connect (ui->fpsSlider, &QSlider::valueChanged, &model, &Model::setFPSValue);
    connect (ui->fpsSlider, &QSlider::valueChanged, this, &MainView::startAnimating);
    connect (ui->fpsSlider, &QSlider::valueChanged,
            this, [this] (int fps) {ui->fpsLabel->setText (QString ("%1").arg (fps));});

    // connection for tool setting
    connect (this, &MainView::setTool, &model, &Model::setTool);

    //connection for updating the canvavas pattern after loading (kyle's issue)
    connect (&model, &Model::setCanvasPattern, this, &MainView::updateCanvasPattern);

    // setting tool button side menus and connections
    QMenu* penMenu = new QMenu ();
    QAction* setPenSize1 = new QAction ("Pen size: 1px", penMenu);
    QAction* setPenSize3 = new QAction ("Pen size: 3px", penMenu);
    QAction* setPenSize5 = new QAction ("Pen size: 5px", penMenu);
    QAction* setPenSize7 = new QAction ("Pen size: 7px", penMenu);
    setPenSize1->setData (1);
    setPenSize3->setData (3);
    setPenSize5->setData (5);
    setPenSize7->setData (7);
    penMenu->addAction (setPenSize1);
    penMenu->addAction (setPenSize3);
    penMenu->addAction (setPenSize5);
    penMenu->addAction (setPenSize7);
    ui->penButton->setMenu (penMenu);

    QMenu* eraserMenu = new QMenu ();
    QAction* setEraserSize1 = new QAction ("Eraser size: 1px", eraserMenu);
    QAction* setEraserSize3 = new QAction ("Eraser size: 3px", eraserMenu);
    QAction* setEraserSize5 = new QAction ("Eraser size: 5px", eraserMenu);
    QAction* setEraserSize7 = new QAction ("Eraser size: 7px", eraserMenu);
    setEraserSize1->setData (1);
    setEraserSize3->setData (3);
    setEraserSize5->setData (5);
    setEraserSize7->setData (7);
    eraserMenu->addAction (setEraserSize1);
    eraserMenu->addAction (setEraserSize3);
    eraserMenu->addAction (setEraserSize5);
    eraserMenu->addAction (setEraserSize7);
    ui->eraserButton->setMenu (eraserMenu);

    connect (setPenSize1, &QAction::triggered, this, &MainView::toolSizeChanged1);
    connect (setPenSize3, &QAction::triggered, this, &MainView::toolSizeChanged3);
    connect (setPenSize5, &QAction::triggered, this, &MainView::toolSizeChanged5);
    connect (setPenSize7, &QAction::triggered, this, &MainView::toolSizeChanged7);
    connect (setEraserSize1, &QAction::triggered, this, &MainView::toolSizeChanged1);
    connect (setEraserSize3, &QAction::triggered, this, &MainView::toolSizeChanged3);
    connect (setEraserSize5, &QAction::triggered, this, &MainView::toolSizeChanged5);
    connect (setEraserSize7, &QAction::triggered, this, &MainView::toolSizeChanged7);
    connect (this, &MainView::sendToolSize, &model, &Model::setToolSize);

    // setting colors yay
    colorButtons[0] = ui->red;
    colorButtons[1] = ui->redOrange;
    colorButtons[2] = ui->orange;
    colorButtons[3] = ui->yellowOrange;
    colorButtons[4] = ui->yellow;
    colorButtons[5] = ui->yellowGreen;
    colorButtons[6] = ui->green;
    colorButtons[7] = ui->blueGreen;
    colorButtons[8] = ui->blue;
    colorButtons[9] = ui->bluePurple;
    colorButtons[10] = ui->purple;
    colorButtons[11] = ui->pink;
    colorButtons[12] = ui->white;
    colorButtons[13] = ui->lightGray;
    colorButtons[14] = ui->darkGray;
    colorButtons[15] = ui->black;
    model.setIndexHue (0, ui->red->palette ().button ().color ().rgb ());
    model.setIndexHue (1, ui->redOrange->palette ().button ().color ().rgb ());
    model.setIndexHue (2, ui->orange->palette ().button ().color ().rgb ());
    model.setIndexHue (3, ui->yellowOrange->palette ().button ().color ().rgb ());
    model.setIndexHue (4, ui->yellow->palette ().button ().color ().rgb ());
    model.setIndexHue (5, ui->yellowGreen->palette ().button ().color ().rgb ());
    model.setIndexHue (6, ui->green->palette ().button ().color ().rgb ());
    model.setIndexHue (7, ui->blueGreen->palette ().button ().color ().rgb ());
    model.setIndexHue (8, ui->blue->palette ().button ().color ().rgb ());
    model.setIndexHue (9, ui->bluePurple->palette ().button ().color ().rgb ());
    model.setIndexHue (10, ui->purple->palette ().button ().color ().rgb ());
    model.setIndexHue (11, ui->pink->palette ().button ().color ().rgb ());
    model.setIndexHue (12, ui->white->palette ().button ().color ().rgb ());
    model.setIndexHue (13, ui->lightGray->palette ().button ().color ().rgb ());
    model.setIndexHue (14, ui->darkGray->palette ().button ().color ().rgb ());
    model.setIndexHue (15, ui->black->palette ().button ().color ().rgb ());

    connect (this, &MainView::setHue, &model, &Model::setHue);
    connect (this, &MainView::setIndexHue, &model, &Model::setIndexHue);
    connect (ui->newColorButton, &QPushButton::clicked, this, &MainView::colorPickerClicked);
    connect (&model, &Model::justANormalSignalNothingToSeeHere
            , this, &MainView::nothingOutOfTheOrdinary);

    // connect save/load/import file menu buttons
    connect (ui->saveFileMenuItem, &QAction::triggered, this, &MainView::saveMenuClicked);
    connect (this, &MainView::saveSelected, &model, &Model::save);
    connect (ui->openFileMenuItem, &QAction::triggered, this, &MainView::openMenuClicked);
    connect (this, &MainView::openSelected, &model, &Model::load);
    connect (ui->importImageMenuItem, &QAction::triggered, this, &MainView::importImageMenuClicked);
    connect (this, &MainView::importImageSelected, &model, &Model::importImage);

    layerCounts.insert (0, 1);
    // make a new layer in the gui to reflect the one created in the model on construction
    guiNewLayer ();
    // testPixMap ();

    emit requestPreview ();
    emit requestCanvas ();

    lastSeenFrame = std::chrono::system_clock::now ();
}

MainView::~MainView ()
{
    delete ui;
    delete canvasScene;
    // delete canvasItem;       // for some reason this one causes a segfault but not the others
    delete previewItem;
    delete previewScene;
}

void MainView::receivePreview (const QPixmap &preview) {
    animating = true;
    lastSeenFrame = std::chrono::system_clock::now ();
    previewItem->setPixmap (preview);
    ui->Preview->setSceneRect (previewItem->boundingRect ());
    ui->Preview->fitInView (previewItem->boundingRect (), Qt::KeepAspectRatio);
    // continue the animation cycle
    QTimer::singleShot (1000, this, [this] {startAnimating (0);});
    emit requestPreview ();
}

void MainView::acknowledgeAnimationFailure () {
    animating = false;
}

// attempts to start the animation cycle if it's not already running
void MainView::startAnimating (int value) {
    Q_UNUSED (value);
    if (std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now () - lastSeenFrame).count () > 5000)
    {
        emit requestPreview ();
    }
}

void MainView::receiveCanvas (const QPixmap &canvas) {
    canvasItem->setPixmap (canvas);

    // QScrollArea* scrollArea = dynamic_cast<QScrollArea*> (ui->frameTabs->widget (frameIndex));
    // QWidget* widget = scrollArea->widget ();
    // QVBoxLayout* layout = dynamic_cast<QVBoxLayout*> (widget->layout ());
    // QGraphicsView* graphicsView = dynamic_cast<QGraphicsView*> (layout->itemAt (layerIndex)->widget ());
    // QGraphicsScene* scene = graphicsView->scene ();

    // scene->clear ();

    // QGraphicsPixmapItem* pixmapItem = scene->addPixmap (canvas);

    // graphicsView->setSceneRect (pixmapItem->boundingRect ());
    // graphicsView->fitInView (pixmapItem->boundingRect (), Qt::KeepAspectRatio);

}

void MainView::receiveLayer (size_t layerIndex, const QImage &layer) {
    QPoint frameLayer (ui->frameTabs->currentIndex (), layerIndex);
    QGraphicsView *view = layersFromFrameLayer[frameLayer];
    if (!view) {
        return;
    }
    QGraphicsScene *scene = view->scene ();
    scene -> clear ();
    QGraphicsPixmapItem *pixmapItem = scene->addPixmap (QPixmap::fromImage (layer));
    view->setSceneRect (pixmapItem->boundingRect ());
    view->fitInView (pixmapItem->boundingRect (), Qt::KeepAspectRatio);
}

void MainView::processUpdate () {
    emit requestCanvas  ();
    // emit requestPreview ();
}

void MainView::previousFrameClicked () {
    int currentTabIndex = ui->frameTabs->currentIndex ();
    if (currentTabIndex == 0){
        ui->frameTabs->setCurrentIndex (ui->frameTabs->count () - 1);
        emit setActiveFrame (ui->frameTabs->count () - 1);
        frameIndex = ui->frameTabs->count () - 1;
    } else {
        ui->frameTabs->setCurrentIndex (currentTabIndex - 1);
        emit setActiveFrame (currentTabIndex - 1);
        frameIndex = currentTabIndex - 1;
    }
}

void MainView::nextFrameClicked (){
    int currentTabIndex = ui->frameTabs->currentIndex ();
    if (currentTabIndex == ui->frameTabs->count () - 1){
        ui->frameTabs->setCurrentIndex (0);
        frameIndex = 0;
        emit setActiveFrame (0);
    } else {
        ui->frameTabs->setCurrentIndex (currentTabIndex + 1);
        frameIndex = currentTabIndex + 1;
        emit setActiveFrame (currentTabIndex + 1);
    }

}

void MainView::newFrameClicked (){
    guiNewFrame ();
    emit addFrame (frameIndex);
}

void MainView::guiNewFrame () {
    int newTabIndex = ui->frameTabs->currentIndex () + 1;
    QScrollArea* newFrame = new QScrollArea ();
    ui->frameTabs->insertTab (newTabIndex, newFrame, QString ("Frame " + QString::number (newTabIndex + 1)));
    for (int i = newTabIndex + 1; i <= ui->frameTabs->count (); i++){
        ui->frameTabs->setTabText (i, QString ("Frame " + QString::number (i + 1)));
    }

    ui->frameTabs->setCurrentIndex (newTabIndex);

    frameIndex = newTabIndex;
    layerIndex = -1;

    // make space for the new frame in the maps
    if (frameIndex < layerCounts.size ()) {
        insertFrame (frameIndex);
    }
    layerCounts.insert (frameIndex, 0);

    guiNewLayer ();
}

void MainView::deleteFrameClicked () {
    int currentTabIndex = ui->frameTabs->currentIndex ();

    ui->frameTabs->removeTab (currentTabIndex);
    for (int i = currentTabIndex; i <= ui->frameTabs->count (); i++) {
        ui->frameTabs->setTabText (i, QString ("Frame " + QString::number (i + 1)));
    }

    if (currentTabIndex < layerCounts.size () - 1) {
        moveFramesDelete (currentTabIndex);
    }
    layerCounts.remove (currentTabIndex);
    emit deleteFrame ();

    if (ui->frameTabs->count () == 0)
        newFrameClicked ();

    ui->frameTabs->setCurrentIndex (0);
    frameIndex = 0;
    layerIndex = 0;

}

void MainView::newLayerClicked () {
    guiNewLayer ();
    emit addLayer (layerIndex);
}

void MainView::guiNewLayer () {
    if (removingLayers){
        removingLayers = false;
        deleteLayerClicked ();
    }

    //if (layerIndex != -1)
    //    emit requestCanvas ();

    int currentTabIndex = ui->frameTabs->currentIndex ();
    QScrollArea* scrollArea = dynamic_cast<QScrollArea*> (ui->frameTabs->widget (currentTabIndex));
    scrollArea->setVerticalScrollBarPolicy (Qt::ScrollBarAlwaysOn);

    // int scrollAreaWidth = scrollArea->maximumWidth ();
    // int scrollAreaHeight = scrollArea->maximumHeight ();

    //change values to be dynamic, TODO: figure out which property is used
    int scrollAreaWidth = 163;
    int scrollAreaHeight = 163;

    QGraphicsScene* newSceneLayer = new QGraphicsScene;
    // QGraphicsView* newViewLayer = new QGraphicsView (newSceneLayer);
    GraphicsLayer* newViewLayer = new GraphicsLayer (newSceneLayer);


    connect (newViewLayer, &GraphicsLayer::updateCurrentLayer, this, [newViewLayer, this] () {
        receiveUpdateCurrentLayer (newViewLayer);
    });


    newViewLayer->setMinimumSize (scrollAreaWidth, scrollAreaHeight);

    QPushButton* removeButton = new QPushButton ("X", newViewLayer);
    removeButton->setFixedSize (25, 25);
    removeButton->setVisible (false);
    removeButton->setEnabled (false);

    removeButton->move (scrollAreaWidth - 25, 0);

    newViewLayer->scene ()->addWidget (removeButton);

    // removeButton->setIcon (QIcon (":/images/start_button.png"));

    connect (removeButton, &QPushButton::clicked, this, [scrollArea, newViewLayer, this] () {
        removeLayerFromScrollArea (scrollArea, newViewLayer);
    });


    QPushButton* visibilityButton = new QPushButton (newViewLayer);
    visibilityButton->setFixedSize (25, 25);
    visibilityButton->setVisible (true);
    visibilityButton->setEnabled (true);
    visibilityButton->setIcon (QIcon (":/images/visible.png"));

    removeButton->setProperty ("buttonType", "remove");
    visibilityButton->setProperty ("buttonType", "visibility");

    connect (visibilityButton, &QPushButton::clicked, this, [newViewLayer, scrollArea, visibilityButton, this] () {
        //removeLayerFromScrollArea (scrollArea, newViewLayer);
        determineVisibility (newViewLayer, scrollArea, visibilityButton);
    });

    //Printing out Frame and Lyaer information Sremvover later
    int scrollWidgetCount = 0;
    QWidget* scrollWidget = scrollArea->widget ();
    if (scrollWidget)
        scrollWidgetCount = scrollWidget->layout ()->count ();

    // newSceneLayer->addText (QString ("Frame: " + QString::number (currentTabIndex + 1) + "\n Layer: " + QString::number (scrollWidgetCount + 1)) + "\n Count: " + QString::number (count++));
    newSceneLayer->addText (QString ("Frame: %1\n Layer: %2\n Count: %3")
                               .arg (currentTabIndex + 1)
                               .arg (scrollWidgetCount + 1)
                               .arg (count++));

    // newSceneLayer->addWidget (removeButton);
    QVBoxLayout* scrollViewLayout = nullptr;

    if (scrollArea->widget ()) {
        QWidget* widget = scrollArea->widget ();
        scrollViewLayout = dynamic_cast<QVBoxLayout*> (widget->layout ());
    }

    else
        scrollViewLayout = new QVBoxLayout ();

    if (layerIndex >= 0 && layerIndex < scrollViewLayout->count ())
    {
        scrollViewLayout->insertWidget (layerIndex + 1, newViewLayer);
    }
    else
    {
        scrollViewLayout->addWidget (newViewLayer);
        newViewLayer->setStyleSheet ("border: 2px solid red;");
        layerIndex = -1;
    }

    QWidget* widget = new QWidget ();
    widget->setLayout (scrollViewLayout);

    scrollArea->setWidget (widget);


    //scrollArea->show ();
    newViewLayer->setFixedWidth (scrollAreaWidth);
    newViewLayer->setFixedHeight (scrollAreaHeight);

    //newViewLayer->updateCurrentLayer (newViewLayer);


    scrollViewLayout->setAlignment (Qt::AlignTop);

    //newViewLayer->setStyleSheet ("border: 2px solid red;");

    scrollArea->verticalScrollBar ()->setValue (scrollArea->verticalScrollBar ()->maximum ());

    QPoint frameLayer (ui->frameTabs->currentIndex (), layerIndex + 1);
    if (layersFromFrameLayer.contains (frameLayer)) {
        insertLayer (frameLayer);
    }
    layerCounts[frameLayer.x ()] = layerCounts[frameLayer.x ()] + 1;

    frameLayerFromLayers[newViewLayer] = frameLayer;
    layersFromFrameLayer[frameLayer] = newViewLayer;

    emit newViewLayer->updateCurrentLayer (newViewLayer);

    emit requestLayer (layerIndex);
}

void MainView::deleteLayerClicked () {
    removingLayers = !removingLayers;
    bool params = removingLayers;

    int currentTabIndex = ui->frameTabs->currentIndex ();
    QScrollArea* scrollArea = dynamic_cast<QScrollArea*> (ui->frameTabs->widget (currentTabIndex));
    QWidget* widgets = scrollArea->widget ();
    QList<QPushButton *> buttons = widgets->findChildren<QPushButton *> ();
    for (auto button : buttons) {
        if (button->property ("buttonType") == "remove") {
            button->setVisible (params);
            button->setEnabled (params);
        }
    }
}


void MainView::tabIndexChanged (int tabIndex) {
    frameIndex = tabIndex;
    emit setActiveFrame (frameIndex);
}


// make space for inserting this layer
void MainView::insertLayer (QPoint &frameLayer)
{
    int layer = frameLayer.y ();
    int frame = frameLayer.x ();
    for (int i = layerCounts[frame] - 1; i >= layer; --i)
    {
        // move the graphic slayer at (frame, i) to (frame, i + 1)
        QPoint newFrameLayer (frame, i + 1);
        QPoint oldFrameLayer (frame, i);
        layersFromFrameLayer[newFrameLayer] = layersFromFrameLayer[oldFrameLayer];
        frameLayerFromLayers[layersFromFrameLayer[newFrameLayer]] = newFrameLayer;
        // frameLayerFromLayers.remove (layersFromFrameLayer[oldFrameLayer]);
        // layersFromFrameLayer[oldFrameLayer] = nullptr;
        // layersFromFrameLayer.remove (oldFrameLayer);
    }
}

void MainView::insertFrame (int frame)
{
    for (int i = layerCounts.size () - 1; i >= frame; --i) {
        for (int j = layerCounts[i] - 1; j >= 0; --j) {
            QPoint newFrameLayer (i + 1, j);
            QPoint oldFrameLayer (i, j);
            layersFromFrameLayer[newFrameLayer] = layersFromFrameLayer[oldFrameLayer];
            frameLayerFromLayers[layersFromFrameLayer[newFrameLayer]] = newFrameLayer;
            frameLayerFromLayers.remove (layersFromFrameLayer[oldFrameLayer]);
            layersFromFrameLayer[oldFrameLayer] = nullptr;
            layersFromFrameLayer.remove (oldFrameLayer);
        }
        // layerCounts[i + 1] = layerCounts[i];
    }
}

void MainView::moveLayersDelete (QPoint &frameLayer) {
    int layer = frameLayer.y ();
    int frame = frameLayer.x ();
    for (int i = layer + 1; i < layerCounts[frame]; ++i)
    {
        // move the graphic slayer at (frame, i) to (frame, i + 1)
        QPoint newFrameLayer (frame, i - 1);
        QPoint oldFrameLayer (frame, i);
        layersFromFrameLayer[newFrameLayer] = layersFromFrameLayer[oldFrameLayer];
        frameLayerFromLayers[layersFromFrameLayer[newFrameLayer]] = newFrameLayer;
        // frameLayerFromLayers.remove (layersFromFrameLayer[oldFrameLayer]);
        // layersFromFrameLayer[oldFrameLayer] = nullptr;
        // layersFromFrameLayer.remove (oldFrameLayer);
    }
}

void MainView::moveFramesDelete (size_t frame) {
    // loop through the frames forward from the deleted index and move them backwards
    for (int i = frame + 1; i < layerCounts.size (); ++i) {
        for (int j = 0; j < layerCounts[i]; ++j) {
            QPoint newFrameLayer (i - 1, j);
            QPoint oldFrameLayer (i, j);
            layersFromFrameLayer[newFrameLayer] = layersFromFrameLayer[oldFrameLayer];
            frameLayerFromLayers[layersFromFrameLayer[newFrameLayer]] = newFrameLayer;
            frameLayerFromLayers.remove (layersFromFrameLayer[oldFrameLayer]);
            layersFromFrameLayer[oldFrameLayer] = nullptr;
            layersFromFrameLayer.remove (oldFrameLayer);
        }
    }
}

void MainView::removeLayerFromScrollArea (QScrollArea* scrollArea, QGraphicsView* newViewLayer) {
    if (nullptr == newViewLayer)
        return;
    deleteLayerClicked ();
    QPoint frameLayer = frameLayerFromLayers[newViewLayer];

    QWidget* widget = scrollArea->widget ();
    QVBoxLayout* layout = dynamic_cast<QVBoxLayout*> (widget->layout ());

    // int indexOfDeletedLayer = layout->indexOf (newViewLayer);
    int indexOfDeletedLayer = frameLayer.y (); //layout->indexOf (newViewLayer);

    layout->removeWidget (newViewLayer);
    emit deleteLayer (indexOfDeletedLayer);

    if ((unsigned) indexOfDeletedLayer < layerCounts[ui->frameTabs->currentIndex ()] - 1) {
        moveLayersDelete (frameLayer);
    }
    layerCounts[frameLayer.x ()] = layerCounts[frameLayer.x ()] - 1;

    if (scrollArea->widget ()->layout ()->count () == 0) {
        removingLayers = false;
    }

    frameLayerFromLayers.remove (newViewLayer);
    layersFromFrameLayer.remove (frameLayer);

    delete newViewLayer;


    if (layout->count () == 0)
    {
        newLayerClicked ();
        removingLayers = false;
        layerIndex = -1;
    }

    else {
        if (layerIndex > indexOfDeletedLayer)
        {
            layerIndex--;
        }
        else if (layerIndex == -1) {
            layerIndex = 0;
        }

        if (layerIndex < layout->count ()) {
            receiveUpdateCurrentLayer (dynamic_cast<QGraphicsView*> (layout->itemAt (layerIndex)->widget ()));
        }

    }
}

void MainView::determineVisibility (QGraphicsView* view, QScrollArea* scrollArea, QPushButton* visibilityButton) {
    // QWidget* widget = scrollArea->widget ();
    // QVBoxLayout* layout = dynamic_cast<QVBoxLayout*> (widget->layout ());
    //qDebug () << layout->indexOf (newViewLa);

    //QPoint frameLayer = frameLayerFromLayers[newViewLayer];

    int indexOfVisibilityLayer = frameLayerFromLayers[view].y ();
    emit setLayerVisibility (indexOfVisibilityLayer);

    //emit changeLayerVisibility (indexOfVisibilityLayer);

    if (visibilityButton->property ("buttonType") == "visibility") {
        visibilityButton->setIcon (QIcon (":/images/invisible.png"));
        visibilityButton->setProperty ("buttonType", "invisibility");
    }
    else if (visibilityButton->property ("buttonType") == "invisibility"){
        visibilityButton->setIcon (QIcon (":/images/visible.png"));
        visibilityButton->setProperty ("buttonType", "visibility");
    }

}

void MainView::changeActiveLayer (size_t index) {
    layerIndex = index;
    QPoint frameLayer (ui->frameTabs->currentIndex (), index);       //might need to change first paramater to frameIndex
    receiveUpdateCurrentLayer (layersFromFrameLayer[frameLayer]);

}

void MainView::receiveUpdateCurrentLayer (QGraphicsView *layer) {

    //this is a slow operateion, but I did try to make it faster and my brain was about to break
    // for (int i = 0; i < ui->frameTabs->count (); ++i) {
    //     QScrollArea* scrollArea = dynamic_cast<QScrollArea*> (ui->frameTabs->widget (i));
    //     QVBoxLayout* scrollLayout = dynamic_cast<QVBoxLayout*> (scrollArea->widget ()->layout ());
    //     for (int j = 0; j < scrollLayout->count (); j++) {
    //         QGraphicsView* currentLayer = dynamic_cast<QGraphicsView*> (scrollLayout->itemAt (j)->widget ());
    //         currentLayer->setStyleSheet ("");
    //     }
    // }

    for (auto &layerPair : layersFromFrameLayer) {
        if (nullptr != layerPair)
            layerPair->setStyleSheet ("");
    }

    if (layer)
        layer->setStyleSheet ("border: 2px solid black;");

    QPoint frameLayer = frameLayerFromLayers[layer];
    int newLayerIndex = frameLayer.y ();
    layerIndex = newLayerIndex;

    emit setActiveLayer (layerIndex);

}

void MainView::colorPickerClicked (){
    QColor hue = QColorDialog::getColor (Qt::white, this);
    thisMethodIsNotSuspicious (hue);
}

void MainView::toolSizeChanged1 (){
    emit sendToolSize (1);
}

void MainView::toolSizeChanged3 (){
    emit sendToolSize (3);
}

void MainView::toolSizeChanged5 (){
    emit sendToolSize (5);
}

void MainView::toolSizeChanged7 (){
    emit sendToolSize (7);
}

void MainView::nothingOutOfTheOrdinary (QRgb hue) {
    if (ui->colorSamplerButton->isChecked ())
        ui->penButton->click ();
    thisMethodIsNotSuspicious (QColor (hue));
}

void MainView::thisMethodIsNotSuspicious (QColor hue) {
    QPushButton *ourButtonFriend = colorButtons[whichHueButtonIsPressed];
    ourButtonFriend->setStyleSheet
        (QString ("background-color: rgb (%1, %2, %3);")
             .arg (hue.red ()).arg (hue.green ()). arg (hue.blue ()));
    ourButtonFriend->update ();
    emit setIndexHue (whichHueButtonIsPressed, hue.rgb ());

}


// slots for tool buttons
void MainView::on_penButton_clicked (bool checked)
{
    if (checked) {
        emit setTool (ToolType::pen);
    }
}

void MainView::on_eraserButton_clicked (bool checked)
{
    if (checked)
        emit setTool (ToolType::eraser);
}

void MainView::on_paintBucketButton_clicked (bool checked)
{
    if (checked)
        emit setTool (ToolType::bucket);
}

void MainView::on_colorSamplerButton_clicked (bool checked)
{
    if (checked)
        emit setTool (ToolType::sampler);
}

// What follows is death and angst, wo be unto he who treads here
void MainView::on_red_clicked (bool checked)
{
    if (checked) {
        emit setHue (0);
        whichHueButtonIsPressed = 0;
    }
}

void MainView::on_redOrange_clicked (bool checked)
{
    if (checked) {
        emit setHue (1);
        whichHueButtonIsPressed = 1;
    }
}

void MainView::on_orange_clicked (bool checked)
{
    if (checked) {
        emit setHue (2);
        whichHueButtonIsPressed = 2;
    }
}

void MainView::on_yellowOrange_clicked (bool checked)
{
    if (checked) {
        emit setHue (3);
        whichHueButtonIsPressed = 3;
    }
}

void MainView::on_yellow_clicked (bool checked)
{
    if (checked) {
        emit setHue (4);
        whichHueButtonIsPressed = 4;
    }
}

void MainView::on_yellowGreen_clicked (bool checked)
{
    if (checked) {
        emit setHue (5);
        whichHueButtonIsPressed = 5;
    }
}

void MainView::on_green_clicked (bool checked)
{
    if (checked) {
        emit setHue (6);
        whichHueButtonIsPressed = 6;
    }
}

void MainView::on_blueGreen_clicked (bool checked)
{
    if (checked) {
        emit setHue (7);
        whichHueButtonIsPressed = 7;
    }
}

void MainView::on_blue_clicked (bool checked)
{
    if (checked) {
        emit setHue (8);
        whichHueButtonIsPressed = 8;
    }
}

void MainView::on_bluePurple_clicked (bool checked)
{
    if (checked) {
        emit setHue (9);
        whichHueButtonIsPressed = 9;
    }
}

void MainView::on_purple_clicked (bool checked)
{
    if (checked) {
        emit setHue (10);
        whichHueButtonIsPressed = 10;
    }
}

void MainView::on_pink_clicked (bool checked)
{
    if (checked) {
        emit setHue (11);
        whichHueButtonIsPressed = 11;
    }
}

void MainView::on_white_clicked (bool checked)
{
    if (checked) {
        emit setHue (12);
        whichHueButtonIsPressed = 12;
    }
}

void MainView::on_lightGray_clicked (bool checked)
{
    if (checked) {
        emit setHue (13);
        whichHueButtonIsPressed = 13;
    }
}

void MainView::on_darkGray_clicked (bool checked)
{
    if (checked) {
        emit setHue (14);
        whichHueButtonIsPressed = 14;
    }
}

void MainView::on_black_clicked (bool checked)
{
    if (checked) {
        emit setHue (15);
        whichHueButtonIsPressed = 15;
    }
}

void MainView::saveMenuClicked () {
    QString fileName = QFileDialog::getSaveFileName (this, "Save a Sprite", QDir::homePath (), ".ssp");
    if (!fileName.isEmpty ()) {
        if (!fileName.endsWith (".ssp")) {
            fileName += ".ssp";
        }
        emit saveSelected (fileName);
    }
}

void MainView::openMenuClicked () {
    QString fileName = QFileDialog::getOpenFileName (this, "Open a Sprite", QDir::homePath ());
    if (!fileName.isEmpty ()) {
        if (fileName.endsWith (".ssp")) {
            ui->frameTabs->clear ();
            frameIndex = 0;
            frameLayerFromLayers.clear ();
            layerIndex = -1;
            layersFromFrameLayer.clear ();
            emit openSelected (fileName);
        }
    }
}

void MainView::importImageMenuClicked () {
    QString fileName = QFileDialog::getOpenFileName (this, "Import an image", QDir::homePath ());
    if (!fileName.isEmpty ()) {
        if (fileName.endsWith (".png", Qt::CaseInsensitive)) {
            emit importImageSelected (fileName);
        }
    }
}

void MainView::updateCanvasPattern (QSize imageSize) {

    MainView::canvasScene = new QGraphicsScene (ui->mainCanvas);
    MainView::previewScene = new QGraphicsScene (ui->Preview);
    previewScene = new QGraphicsScene ();
    previewItem = new QGraphicsPixmapItem ();
    canvasScene = new QGraphicsScene ();
    canvasItem = new QGraphicsPixmapItem ();
    canvasScene->setBackgroundBrush (QBrush (Qt::darkGray));
    previewScene->setBackgroundBrush (QBrush (Qt::darkGray));
    Checkerboard *checkers = new Checkerboard (QRectF (QPointF (0, 0), imageSize));
    canvasScene->addItem (checkers);
    previewScene->addItem (previewItem);
    canvasScene->addItem (canvasItem);
    ui->Preview->setScene (previewScene);
    ui->mainCanvas->setScene (canvasScene);
    emit requestCanvas ();
}

void MainView::helpActionTriggered (QAction* action){
    QMessageBox helpBox;

    if (action->text () == "Pencil"){
        helpBox.setWindowTitle ("Pencil");
        helpBox.setText ("This tool is used to draw colored pixels on the canvas. To use it, click on the pencil icon in the top left. "
                        "The currently selected color will be used while you are drawing. If you want to draw with a certain size, "
                        "click on the drop down menu that is on the right side of the pencil icon to specify from one of four predetermined "
                        "sizes. The pencil will only draw on any currently active layers, and only within the current frame.");
    }
    else if (action->text () == "Eraser"){
        helpBox.setWindowTitle ("Eraser");
        helpBox.setText ("This tool will erase drawn pixels. To use it, click on the eraser icon in the top left. "
                        "The Eraser is independent of the currently selected color. If you want to erase with a certain size, "
                        "click on the drop down menu that is on the right side of the eraser icon to specify from one of four predetermined "
                        "sizes. The eraser will only erase on any currently active layers, and only within the current frame.");
    }
    else if (action->text () == "Fill Tool"){
        helpBox.setWindowTitle ("Fill Tool");
        helpBox.setText ("This tool is used to fill in a large, enclosed area with one click. To use it, click on the paint bucket icon in the top left. "
                        "The currently selected color will be used while you are drawing. The paint bucket will only replace connected pixels with the same color as the clicked pixel. "
                        "The Fill Tool will only fill pixels on any currently active layers, and only within the current frame.");
    }
    else if (action->text () == "Color Sampler"){
        helpBox.setWindowTitle ("Color Sampler");
        helpBox.setText ("This tool is used to reassign the current color to the color of a clicked pixel in the canvas. To use it, click on the eyedropper "
                        "icon in the top left. After clicking on a pixel, the currently selected color will be overwritten. The Color Sampler will not modify any pixels on "
                        "the canvas, even if they are in the currently selected layers or frame.");
    }
    else if (action->text () == "Color Palette"){
        helpBox.setWindowTitle ("Color Palette");
        helpBox.setText ("The Color Palette has 16 color slots that come with default values. There will always be a currently selected color "
                        "that can be changed. You can change to a different currently selected color by clicking on one of the other 15 colors. If you wish to change the color "
                        "value of the current color, you can use the Color Selector, or the Color Sampler.");
    }
    else if (action->text () == "Color Selector"){
        helpBox.setWindowTitle ("Color Selector");
        helpBox.setText ("The Color Selector allows you to choose a color with an RGB picker. The color that you pick will replace the currently selected "
                        "color in the Color Palette. ");
    }
    else if (action->text () == "Canvas"){
        helpBox.setWindowTitle ("Canvas");
        helpBox.setText ("The Canvas allows you to edit pixels on it. The Canvas will display the composition of all currently visible layers in the "
                        "currently active frame. Editing pixels on the canvas will change the same pixel for all currently active layers.");

    }
    else if (action->text () == "Animation Playback"){
        helpBox.setWindowTitle ("Animation Playback");
        helpBox.setText ("The Animation Playback will display an animation of all frames at a speed controlled by the FPS slider. "
                        "If there is only one frame, the Animation Playback will be stagnant. With any more than one frame, it will display all frames in order continuously.");
    }
    else if (action->text () == "FPS Slider"){
        helpBox.setWindowTitle ("FPS Slider");
        helpBox.setText ("The FPS Slider allows you to change how quickly the Animation Playback plays. It ranges from 1 frame per second to 24 frames per second.");
    }
    else if (action->text () == "Previous Frame"){
        helpBox.setWindowTitle ("Previous Frame");
        helpBox.setText ("The Previous Frame button will change the currently selected frame to the frame that comes directly before it in the playback cycle. "
                        "If the first frame is the currently selected frame, the last frame will be moved to. If there is only one frame it will stay on that frame.");
    }
    else if (action->text () == "Next Frame"){
        helpBox.setWindowTitle ("Next Frame");
        helpBox.setText ("The Next Frame button will change the currently selected frame to the frame that comes directly after it in the playback cycle. "
                        "If the last frame is the currently selected frame, the first frame will be moved to. If there is only one frame it will stay on that frame.");
    }
    else if (action->text () == "New Frame"){
        helpBox.setWindowTitle ("New Frame");
        helpBox.setText ("The New Frame button will create a new frame in the slot directly after the current frame. The new frame will have layers "
                        "independent of the previous frame.");
    }
    else if (action->text () == "New Layer"){
        helpBox.setWindowTitle ("New Layer");
        helpBox.setText ("The New Layer button will create a new layer in the slot directly after the current layer. The new layer will be a blank canvas, and "
                        "independent of the previous layer. The new layer will have visibility turned on by default.");
    }
    else if (action->text () == "Delete Frame"){
        helpBox.setWindowTitle ("Delete Frame");
        helpBox.setText ("The Delete Frame button will delete the currently selected frame. That frame will be removed from the animation frame cycle, and "
                        "will no longer be played in the frame animation. If there is only one frame, then that frame will be replaced with a new frame.");
    }
    else if (action->text () == "Delete Layer"){
        helpBox.setWindowTitle ("Delete Layer");
        helpBox.setText ("The Delete Layer button will display Xs on all layers in the current frame, and allow you to click that X on any layer you want to "
                        "delete. The Delete Layer button is used similar to a switch, so you will need to click the Delete Layer button again after you are done deleting "
                        "layers. Any deleted layers will be removed from the frame preview.");
    }
    else{
        helpBox.setWindowTitle ("Visibility Indicator");
        helpBox.setText ("The Visibility Indicator is a button that is attached to every layer. By default it is in the on state, and it "
                        "allows you to disable that layer on the canvas. Any layers that are not visible will not be edited by changes made on the canvas. "
                        "Any layers that are visible will be affected by changes made on the canvas.");
    }
    helpBox.exec ();
}
