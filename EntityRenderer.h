#ifndef ENTITYRENDERER_H
#define ENTITYRENDERER_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPixmap>
#include <memory>
#include <QMap>
#include <QGraphicsPixmapItem>
#include <QGraphicsTextItem>
#include "SpriteManager.h"
#include "EnemyModel.h"
#include "TileModel.h"
#include "DIRECTION.h"
#include <QTimer>

class GameModel;

class EntityRenderer: public QObject {
    Q_OBJECT
public:
    EntityRenderer(QGraphicsScene* scene, SpriteManager* spriteManager, QObject* parent = nullptr);

    void renderProtagonist(int x, int y, const QRect& spriteRect, Direction direction, bool useDeathSpriteSheet);
    void renderEnemies(const QVector<std::shared_ptr<EnemyModel>>& enemies);
    void renderHealthPacks(const QVector<std::shared_ptr<TileModel>>& healthPacks);
    void reRenderEnemies(const QVector<std::shared_ptr<EnemyModel>>& enemies);

    void clearAllEntities();

    void showDamageEffect();
    void showHealingEffect();
private slots:
    void handleHealthChange(int damage);

private:
    QTimer* protagonistHitTimer = nullptr;
    QTimer* healingAnimTimer = nullptr;
    bool isProtagonistHit = false;
    bool isHealing = false;

    QGraphicsScene* scene;
    GameModel& model;
    SpriteManager* spriteManager;
    std::shared_ptr<QGraphicsPixmapItem> protagonistItem;
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsPixmapItem>> enemyItems;
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsTextItem>> enemyHealthTexts;
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsPixmapItem>> healthPackItems;
    QSet<std::pair<int, int>> enemyPositions;
    QSet<std::pair<int, int>> healthPackPositions;
    QPixmap flipPixmapHorizontally(const QPixmap& pixmap);
    QSet<std::pair<int, int>> visitedTiles;
};

#endif // ENTITYRENDERER_H
