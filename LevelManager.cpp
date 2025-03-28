#include "LevelManager.h"
#include "QDebug"

LevelManager::LevelManager(QObject* parent)
    : QObject(parent){
}

void LevelManager::loadLevel(int levelIndex) {
    if (levelIndex >= 0) {
        qDebug() << "Before getOrCreateLevel";
        auto level = getOrCreateLevel(levelIndex);
        qDebug() << "After getOrCreateLevel - level valid:" << (level != nullptr);

        currentLevel = level;
        currentLevelIndex = levelIndex;

        qDebug() << "Before emit levelLoaded";
        emit levelLoaded(level);
        qDebug() << "After emit levelLoaded";

        level->getProtagonist()->setHealth(100);
        level->getProtagonist()->setEnergy(100);
    }
}

void LevelManager::handleLevelChange(int targetLevel) {
    loadLevel(targetLevel);
}

void LevelManager::clearLevelStates() {
    levelStates.clear();
}

std::shared_ptr<Level> LevelManager::getCurrentLevel(){
    return currentLevel;
}

std::shared_ptr<Level> LevelManager::getOrCreateLevel(int index) {
    auto it = levelStates.find(index);
    if (it != levelStates.end()) {
        return it->second;
    }

    auto level = std::make_shared<Level>(index);
    level->createWorld(getLevelPath(index), getOverlayPath(index), index);

    try {
        switch (index) {
        case 1:
            if (level->getProtagonist()) {
                level->getProtagonist()->setPos(0, 7);
            }
            level->setNextDoor(25, 0);
            break;
        case 2:
            if (level->getProtagonist()) {
                level->getProtagonist()->setPos(10, 10);
            }
            level->setPrevDoor(0, 11);
            level->setNextDoor(28, 4);
            break;
        case 3:
            if (level->getProtagonist()) {
                level->getProtagonist()->setPos(0, 0);
            }
            level->setPrevDoor(0, 0);
            break;
        default:
            break;
        }
    } catch (const std::out_of_range& oor) {
        qDebug() << "Error setting door: " << oor.what();
    }

    levelStates[index] = level;
    return level;
}

QString LevelManager::getLevelPath(int index) const {
    return QString(":/images/worldmapLVL%1.png").arg(index);
}

QString LevelManager::getOverlayPath(int index) const {
    return QString(":/images/overlay%1.png").arg(index);
}
