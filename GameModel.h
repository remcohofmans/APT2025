#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <memory>
#include <QObject>
#include "Level.h"

class LevelManager;

class GameModel: public QObject {
    Q_OBJECT

signals:
    void levelChangeRequested(int levelIndex);
    void worldChanged();

public:
    static GameModel& getInstance();
    std::shared_ptr<LevelManager> getLevelManager() const {return levelManager;};
    std::shared_ptr<Level> getCurrentLevel();
    void initializeLevelManager();

private:
    GameModel() = default;
    static GameModel* instance;

    std::shared_ptr<LevelManager> levelManager;
};

#endif // GAMEMODEL_H
