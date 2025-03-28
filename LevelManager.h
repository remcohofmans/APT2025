#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include <memory>
#include "Level.h"


class LevelManager: public QObject{
    Q_OBJECT
private:
    std::map<int, std::shared_ptr<Level>> levelStates;
    int currentLevelIndex{0};

    std::shared_ptr<Level> getOrCreateLevel(int index);
    QString getLevelPath(int index) const;
    QString getOverlayPath(int index) const;

    std::shared_ptr<Level> currentLevel;

public slots:
    void handleLevelChange(int targetLevel);

public:
    explicit LevelManager(QObject* parent = nullptr);
    void loadLevel(int levelIndex);
    void clearLevelStates();

    std::shared_ptr<Level> getCurrentLevel();

signals:
    void levelLoaded(std::shared_ptr<Level> level);
};

#endif //LEVELMANAGER_H
