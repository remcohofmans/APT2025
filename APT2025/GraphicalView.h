#ifndef GRAPHICALVIEW_H
#define GRAPHICALVIEW_H

#include <QGraphicsView>
#include <set>
#include "View.h"
#include "AnimationController.h"
#include "SpriteManager.h"
#include "EntityRenderer.h"
#include "OverlayView.h"

class GraphicalView : public View {
    Q_OBJECT

public:
    explicit GraphicalView(QWidget* parent = nullptr);
    void visualizeWorld() override;
    void makeOverlay(const QString& imagePath);
    void toggleOverlay();
    void makeTiles(QVector<std::shared_ptr<TileModel>> tiles);
    void clearScene();
    void refreshEnemies();
    void setPathTiles(const std::vector<std::pair<int, int>>& newPathTiles);

    std::shared_ptr<AnimationController> getAnimationController()const {return animationController;};

protected:
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

signals:
    void tileClicked(int x, int y);

private slots:
    void onWorldLoaded();
public slots:
    void updateTileColors();
    void refreshScene();

private:
    std::shared_ptr<AnimationController> animationController;
    std::unique_ptr<SpriteManager> spriteManager;
    std::unique_ptr<EntityRenderer> entityRenderer;
    float zoomFactor;

    std::vector<std::shared_ptr<QGraphicsPixmapItem>> protagonistItems;

    std::vector<std::shared_ptr<QGraphicsPixmapItem>> poisonEnemyItems;
    std::vector<std::shared_ptr<QGraphicsPixmapItem>> xEnemyItems;
    QVector<std::shared_ptr<QGraphicsTextItem>> healthTextItems;

    QVector<QGraphicsRectItem*> tileRects;

    std::shared_ptr<QGraphicsPixmapItem> protagonistItem;
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsPixmapItem>> enemyItems;
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsTextItem>> enemyHealthTexts;
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsPixmapItem>> healthPackItems;

    std::set<std::pair<int, int>> pathTiles;

    // QGraphicsScene *scene;
    qreal gridWidth;
    bool overlayToggle;
    OverlayView* overlay;
};

#endif // GRAPHICALVIEW_H
