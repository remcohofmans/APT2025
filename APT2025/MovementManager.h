#ifndef MOVEMENTMANAGER_H
#define MOVEMENTMANAGER_H

#include "GameModel.h"
#include "CollisionHandler.h"
#include "DIRECTION.h"
#include "TileModel.h"


class MovementManager: public QObject {
   Q_OBJECT

public:
    explicit MovementManager();

    int attemptMove(Direction dir);
    int attemptMoveToTile(std::pair<int, int>);
    QString directionToString(Direction dir) const;
    static inline std::pair<int, int> posOfTarget = {0, 0}; // Inline initialization (C++17 or later)
    CollisionHandler& getCollisionHandler() { return collisionHandler; }

private:
    GameModel& model;
    CollisionHandler collisionHandler;

    std::pair<int, int> calculateTargetPosition(Direction dir) const;
    bool performMove(TileModel& targetTile);
    bool isValidMove(const TileModel& targetTile) const;
    bool hasHealthPack(const TileModel& tileToCheck) const;
};
#endif // MOVEMENTMANAGER_H
