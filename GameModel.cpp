#include "GameModel.h"
#include "LevelManager.h"

GameModel* GameModel::instance = nullptr;

GameModel& GameModel::getInstance() {
    if (!instance) {
        instance = new GameModel();
    }
    return *instance;
}

std::shared_ptr<Level> GameModel::getCurrentLevel(){
    return levelManager->getCurrentLevel();
}

void GameModel::initializeLevelManager() {
    levelManager = std::make_shared<LevelManager>();

    connect(this, &GameModel::levelChangeRequested, levelManager.get(), &LevelManager::handleLevelChange);
}
