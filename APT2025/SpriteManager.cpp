#include "SpriteManager.h"
#include <QDebug>

SpriteManager::SpriteManager() {}

bool SpriteManager::loadSprites() {
    bool success = true;

    if (!protagonistSpritesheet.load(":/images/warrior_blue.png")) {
        qDebug() << "Failed to load protagonist spritesheet";
        success = false;
    }
    if (!enemySprite.load(":/images/enemy.png")) {
        qDebug() << "Failed to load enemy image";
        success = false;
    }
    if (!healthPackSprite.load(":/images/meat.png")) {
        qDebug() << "Failed to load health pack image";
        success = false;
    }
    if (!poisonEnemySprite.load(":/images/barrel_single.png")) {
        qDebug() << "Failed to load poison enemy image";
        success = false;
    }
    if (!xEnemySprite.load(":/images/Xenemy.png")) {
        qDebug() << "Failed to load xEnemy image";
        success = false;
    }
    if (!defeatedEnemySprite.load(":/images/defeated.png")) {
        qDebug() << "Failed to load defeated image";
        success = false;
    }
    if(!defeatedSpriteSheet.load(":/images/dead.png")){
        qDebug() << "Failed to load defeatedSpriteSheet image";
        success = false;
    }

    return success;
}
