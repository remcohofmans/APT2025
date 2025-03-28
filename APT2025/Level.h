#ifndef LEVEL_H
#define LEVEL_H

#include <memory>
#include "TileModel.h"
#include "EnemyModel.h"
#include "ProtagonistModel.h"
#include "ChasingEnemyModel.h"


class Level: public QObject{
    Q_OBJECT
private:

    QVector<std::shared_ptr<EnemyModel>> enemyModels;
    QVector<std::shared_ptr<TileModel>> healthPackModels;
    QVector<std::shared_ptr<TileModel>> tileModels;
    std::shared_ptr<ProtagonistModel> protagonistModel;
    QString overlay;

    int mapRows;
    int mapCols;
    int level;

    std::vector<std::shared_ptr<ChasingEnemyModel>> chasingEnemies;
    QTimer* chasingEnemyTimer;
    void startChasingEnemyTimer();

public slots:
    void HandlePoisonSurroundingTiles(const std::pair<int, int>& enemyPosition, int poisonRange, float poisonDuration);
    void checkProtagonistPoisonDamage();

signals:
    void poisonStatusChanged();
    void protagonistDefeated();
    void enemyMoved();

public:
    Level(int levelIndex);

    void moveTowards(int targetX, int targetY, ChasingEnemyModel& enemy);
    bool inAttackRange();

    void updateChasingEnemies();

    void createWorld(const QString &worldPath, const QString &overlayPath, int levelIndex);
    void removeHealthPack(int x, int y);

    const QVector<std::shared_ptr<EnemyModel>> &getEnemies() const {return enemyModels;};
    const QVector<std::shared_ptr<TileModel>>& getHealthPacks()const {return healthPackModels;};
    const QVector<std::shared_ptr<TileModel>> &getTileModels()const {return tileModels;};
    std::shared_ptr<ProtagonistModel>  &getProtagonist(){return protagonistModel;};

    int getMapRows() const { return mapRows;};
    int getMapCols() const { return mapCols;};
    int getLevelIndex() const { return level;};
    QString getOverlay() const { return overlay;};
    TileModel& getTileAt(int xPos, int yPos) const;

    void setNextDoor(int x, int y);
    void setPrevDoor(int x, int y);

    void decrementAllPoisonLevels();
    void startPoisonDecrementTimer();
    void removeHealthPackAt(int x, int y);
};


#endif // LEVEL_H
