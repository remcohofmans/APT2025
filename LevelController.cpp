#include "LevelController.h"
#include <QDebug>
#include "GameModel.h"

LevelController::LevelController(QObject* parent): QObject(parent){
    qDebug() << "LevelController created";
    createLevels();
    connect(&model, &GameModel::levelChangeRequested, this, &LevelController::handleLevelChange);

}

std::shared_ptr<LevelModel> LevelController::makeLevel(int index, const QString& mapPath, const QString& overlayPath) {
    std::shared_ptr<LevelModel> level = std::make_shared<LevelModel>();
    level->createWorld(mapPath, overlayPath, index);
    return level;
}

void LevelController::createLevels()
{
    //world 1
    std::shared_ptr<LevelModel> level1 = makeLevel(1,":/images/worldmapLVL3.png", ":/images/overlay2.png");
    level1->setProtagonistStartPosition(0,0);
    level1->setNextDoor(3,3);
    levels.push_back(level1);

    //world 2
    std::shared_ptr<LevelModel> level2 = makeLevel(2,":/images/worldmapLVL2.png", ":/images/overlay1.png");
    level2->setProtagonistStartPosition(6,6);
    level2->setPrevDoor(6, 8);
    levels.push_back(level2);

    //world 3
    std::shared_ptr<LevelModel> level3 = makeLevel(3,":/images/worldmapLVL3.png", ":/images/overlay3.png");
    level3->setProtagonistStartPosition(5,5);
    level3->setNextDoor(26,3);
    levels.push_back(level3);
}

std::shared_ptr<LevelModel> LevelController::getLevelState(int levelIndex){
    auto it = levelStates.find(levelIndex);
    if(it == levelStates.end()){
        for(const auto& level: levels){
            if(level->getLevelIndex() == levelIndex){
                levelStates[levelIndex] = std::make_shared<LevelModel>(*level);
                return levelStates[levelIndex];
            }
        }
        throw std::out_of_range("level not found");
    }
    return it->second;
}

void LevelController::loadLevel(int levelIndex) {
    try {
        if (model.getCurrentLevel() > 0) {
            auto currentState = levelStates[model.getCurrentLevel()];
            currentState->setEnemies(model.getEnemies());
            currentState->setHealthPacks(model.getHealthPacks());
            currentState->setTileModels(model.getTileModels());
            currentState->setProtagonist(model.getProtagonist());
        }

        auto newLevelState = getLevelState(levelIndex);
        model.loadWorld(newLevelState);

    } catch (const std::out_of_range& e) {
        qDebug() << "Failed to load level:" << levelIndex << "-" << e.what();
    }
}

void LevelController::updateCurrentLevelState() {
    int currentLevel = model.getCurrentLevel();
    if (currentLevel > 0) {
        auto state = levelStates[currentLevel];
        state->setEnemies(model.getEnemies());
        state->setHealthPacks(model.getHealthPacks());
        state->setTileModels(model.getTileModels());
        state->setProtagonist(model.getProtagonist());
    }
}

void LevelController::loadNextLevel(){
    int nextLevel = model.getCurrentLevel() + 1;
    if(nextLevel <= static_cast<int>(levels.size())){
        loadLevel(nextLevel);
    } else{
        qDebug() << "No more levels available";
    }
}

void LevelController::loadPrevLevel(){
    int prevLevel = model.getCurrentLevel() -1;
    if(prevLevel > 0){
        loadLevel(prevLevel);
    } else{
        qDebug() << "Already On first level";
    }
}

void LevelController::handleLevelChange(int targetLevel){
    qDebug() << "Signal to switch level trigegred";
    loadLevel(targetLevel);
}
