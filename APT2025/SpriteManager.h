#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <QPixmap>
#include <QString>

class SpriteManager {
public:
    SpriteManager();
    bool loadSprites();
    const QPixmap& getProtagonistSpritesheet() const { return protagonistSpritesheet; };
    const QPixmap& getEnemySprite() const { return enemySprite; };
    const QPixmap& getHealthPackSprite() const { return healthPackSprite; };
    const QPixmap& getPoisonEnemySprite() const { return poisonEnemySprite; };
    const QPixmap& getXEnemySprite() const { return xEnemySprite; };
    const QPixmap& getDefeatedSprite() const { return defeatedEnemySprite; };
    const QPixmap& getDeathSpritesheet() const {return defeatedSpriteSheet;};

private:
    QPixmap protagonistSpritesheet;
    QPixmap enemySprite;
    QPixmap healthPackSprite;
    QPixmap poisonEnemySprite;
    QPixmap xEnemySprite;
    QPixmap defeatedEnemySprite;
    QPixmap defeatedSpriteSheet;
};

#endif // SPRITEMANAGER_H
