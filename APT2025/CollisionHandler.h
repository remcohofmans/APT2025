#ifndef COLLISIONHANDLER_H
#define COLLISIONHANDLER_H

#include <memory>
#include "EnemyModel.h"
#include "TileModel.h"
#include "QTimer"

class GameModel;

class CollisionHandler: public QObject {
    Q_OBJECT
private:
    GameModel& model;
    std::shared_ptr<EnemyModel> currentTarget;
    QTimer* combatTimer;

public:
    explicit CollisionHandler(QObject* parent = nullptr);

    std::shared_ptr<EnemyModel> findEnemyAt(const TileModel& tileToCheck);
    bool isEnemyAt(const TileModel& targetTile);
    void startAttackOnEnemy(std::shared_ptr<EnemyModel> enemy);
    void processCombatTick();
    void stopAttacking();
    bool isInCombat() const;
    std::shared_ptr<EnemyModel> getCurrentTarget() const;

signals:
    void combatEnded();
};

#endif // COLLISIONHANDLER_H
