#include "GraphicalView.h"
#include "InputController.h"
#include "LevelManager.h"
#include "qevent.h"
#include "LevelManager.h"
#include <QGraphicsColorizeEffect>


GraphicalView::GraphicalView(QWidget* parent)
    : View(parent)
    , zoomFactor(1.0f)
    , overlayToggle(false)
{
    spriteManager = std::make_unique<SpriteManager>();
    if (!spriteManager->loadSprites()) {
        qDebug() << "Failed to load some sprites";
    }
    connect(model.getLevelManager().get(), &LevelManager::levelLoaded, this, &GraphicalView::onWorldLoaded);


    animationController = std::make_shared<AnimationController>(this);
    connect(animationController.get(), &AnimationController::frameChanged,
            this, &GraphicalView::visualizeWorld);

    entityRenderer = std::make_unique<EntityRenderer>(scene, spriteManager.get());

    setRenderHint(QPainter::Antialiasing);

    connect(model.getCurrentLevel()->getProtagonist().get(), &ProtagonistModel::statusChanged,
            animationController.get(), &AnimationController::handleStatusChange);
    connect(&model, &GameModel::worldChanged, this, &GraphicalView::refreshScene);

    makeTiles(model.getCurrentLevel()->getTileModels());
}

void GraphicalView::onWorldLoaded() {
    clearScene();
    makeTiles(model.getCurrentLevel()->getTileModels());
    disconnect(nullptr, nullptr, animationController.get(), nullptr);
    connect(model.getCurrentLevel()->getProtagonist().get(), &ProtagonistModel::statusChanged,
            animationController.get(), &AnimationController::handleStatusChange);


    if (model.getCurrentLevel()->getOverlay() != "") {
        makeOverlay(model.getCurrentLevel()->getOverlay());
    }
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void GraphicalView::clearScene() {
    // Clear all entities
    entityRenderer->clearAllEntities();

    // Clear all tiles
    QList<QGraphicsItem*> sceneItems = scene->items();
    for (QGraphicsItem* item : sceneItems) {
        if (auto rectItem = dynamic_cast<QGraphicsRectItem*>(item)) {
            scene->removeItem(rectItem);
            delete rectItem;
        }
    }
}

void GraphicalView::visualizeWorld() {
    auto protagonist = model.getCurrentLevel()->getProtagonist();
    bool isDeathAnimation = protagonist->getStatus() == ProtagonistStatus::DEFEATED;

    entityRenderer->renderProtagonist(
        model.getCurrentLevel()->getProtagonist()->getXPos(),
        model.getCurrentLevel()->getProtagonist()->getYPos(),
        animationController->getSpriteRect(),
        model.getCurrentLevel()->getProtagonist()->getDirection(),
        isDeathAnimation
        );

    entityRenderer->renderEnemies(model.getCurrentLevel()->getEnemies());
    entityRenderer->renderHealthPacks(model.getCurrentLevel()->getHealthPacks());
}

void GraphicalView::makeTiles(QVector<std::shared_ptr<TileModel>> tileModels) {
    tileRects.clear();
    for (const auto& tile : tileModels) {
        float value = tile->getValue();
        int x = tile->getXPos();
        int y = tile->getYPos();

        int grayValue = static_cast<int>(qBound(0.0f, value * 255.0f, 255.0f));
        QColor color;

        bool isInPath = pathTiles.find({x, y}) != pathTiles.end();

        if (tile->isPoisoned()) {
            if (isInPath) {
                color = QColor(grayValue * 0.7, grayValue, grayValue * 0.9);  // Greenish tint for poisoned path tiles
            } else {
                color = QColor(grayValue * 0.7, grayValue, grayValue * 0.7);  // Normal poisoned tile
            }
        } else {
            if (isInPath) {
                color = QColor(grayValue * 0.9, grayValue * 0.9, grayValue);  // Yellowish tint for path tiles
            } else {
                color = QColor(grayValue, grayValue, grayValue);  // Normal tile
            }
        }

        if(tile->isDoor()){
            color = QColor(grayValue, grayValue, grayValue*.1);
        } else{
            color = QColor(grayValue, grayValue, grayValue);
        }

        QGraphicsRectItem* rectItem = scene->addRect(x * 10, y * 10, 10, 10, QPen(Qt::NoPen), QBrush(color));
        rectItem->setZValue(3);
        tileRects.push_back(rectItem);
    }

    gridWidth = ((tileModels.back()->getXPos()) + 1) * 10;
}

void GraphicalView::updateTileColors() {
    auto tileModels = model.getCurrentLevel()->getTileModels();
    for (int i = 0; i < tileModels.size(); i++) {
        const auto& tile = tileModels[i];
        int grayValue = static_cast<int>(qBound(0.0f, tile->getValue() * 255.0f, 255.0f));

        bool isInPath = pathTiles.find({tile->getXPos(), tile->getYPos()}) != pathTiles.end();

        QColor color;
        if (tile->isPoisoned()) {
            if (isInPath) {
                color = QColor(grayValue * 0.7, grayValue, grayValue * 0.9);  // Greenish tint for poisoned path tiles
            } else {
                color = QColor(grayValue * 0.7, grayValue, grayValue * (1 - tile->getPoisonLevel()));  // Normal poisoned tile
            }
        } else {
            if (isInPath) {
                color = QColor(grayValue * 0.9, grayValue * 0.9, grayValue);  // Yellowish tint for path tiles
            } else {
                color = QColor(grayValue, grayValue, grayValue);  // Normal tile
            }
        }

        tileRects[i]->setBrush(QBrush(color));
    }
}

void GraphicalView::setPathTiles(const std::vector<std::pair<int, int>>& newPathTiles) {
    pathTiles.clear();
    for (const auto& coord : newPathTiles) {
        pathTiles.insert(coord);
    }
    updateTileColors();
}

void GraphicalView::wheelEvent(QWheelEvent* event) {
    const double zoomFactor = 1.2;
    if (event->angleDelta().y() > 0) {
        scale(zoomFactor, zoomFactor);
    } else {
        scale(1.0 / zoomFactor, 1.0 / zoomFactor);
    }
    event->accept();
}

void GraphicalView::mousePressEvent(QMouseEvent* event) {

    // Prevent focus stealing by setting focus policy to NoFocus
    setFocusPolicy(Qt::NoFocus);

    const int TILE_SIZE = 10;

    // Convert the mouse click position to scene coordinates
    QPointF scenePos = mapToScene(event->pos());

    // Calculate grid coordinates
    int targetX = static_cast<int>(floor(scenePos.x() / TILE_SIZE));
    int targetY = static_cast<int>(floor(scenePos.y() / TILE_SIZE));

    // Validate coordinates are within bounds
    if (targetX >= 0 && targetY >= 0) {
        auto tile = model.getCurrentLevel()->getTileAt(targetX, targetY);
        if (tile.getValue() > 1) {
            qDebug() << "Moving to tile [" << targetX << "," << targetY
                     << "] failed. Tile is not accessible by protagonist.";
        } else {
            model.getCurrentLevel()->getProtagonist()->setPos(targetX, targetY);
            qDebug() << "Mouse clicked - Moving to tile:" << targetX << "," << targetY;
            emit tileClicked(targetX, targetY);

            // Ensure the InputController regains focus
            if (auto inputController = findChild<InputController*>()) {
                inputController->setFocus();
            }
        }
    }

    // Don't call the base class implementation to prevent focus changing
    event->accept();
}

void GraphicalView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void GraphicalView::makeOverlay(const QString& imagePath) {
    overlay = new OverlayView();

    //Set correct overlay image
    overlay->setOverlayImage(imagePath);

    // Calculate the scale factor based on the gird and overlay
    QRectF overlayRect = overlay->sceneBoundingRect();
    qreal overlayWidth = overlayRect.width();
    qreal scaleFactor = gridWidth / overlayWidth;

    // Apply the scale
    overlay->setScale(scaleFactor);

    // Add the overlay to the scene
    scene->addItem(overlay);
    overlay->setZValue(0);
}

void GraphicalView::toggleOverlay() {
    overlay->setZValue(overlayToggle ? 0 : 1000);
    overlayToggle = !overlayToggle;
}

void GraphicalView::refreshScene() {
    qDebug() << "Scene refreshed.";
    visualizeWorld();
}

void GraphicalView::refreshEnemies(){
    entityRenderer->reRenderEnemies(model.getCurrentLevel()->getEnemies());
}

