#include "MovementManager.h"
#include "qdebug.h"
#include "CollisionHandler.h"

MovementManager::MovementManager()
    : model(GameModel::getInstance()), collisionHandler() {}

int MovementManager::attemptMove(Direction dir) {
    auto newPos = calculateTargetPosition(dir);
    auto currentPos = std::make_pair(
        model.getCurrentLevel()->getProtagonist()->getXPos(),
        model.getCurrentLevel()->getProtagonist()->getYPos()
        );

    // Only stop combat if we're moving to a different tile that isn't our combat target
    if (newPos != currentPos && collisionHandler.isInCombat() && newPos != posOfTarget) {
        collisionHandler.stopAttacking();
    }

    int moveResult = attemptMoveToTile(newPos);

    qDebug() << "In attemptMove()"
             << "Health:" << model.getCurrentLevel()->getProtagonist()->getHealth()
             << "Energy:" << model.getCurrentLevel()->getProtagonist()->getEnergy();

    return moveResult;
}

bool MovementManager::hasHealthPack(const TileModel& tileToCheck) const {
    QVector<std::shared_ptr<TileModel>> healthPacks = model.getCurrentLevel()->getHealthPacks();
    for (const auto& healthPack : healthPacks) {
        if (*healthPack == tileToCheck) { // Assuming TileModel implements operator==
            return true;
        }
    }
    return false;
}

int MovementManager::attemptMoveToTile(std::pair<int, int> targetTilePosition) {
    try {
        // Attempt to retrieve the target tile
        TileModel targetTile = model.getCurrentLevel()->getTileAt(targetTilePosition.first, targetTilePosition.second);
        qDebug() << "In attemptMoveToTile";

        // Check if there's an enemy at the target tile
        auto enemy = collisionHandler.findEnemyAt(targetTile);
        if (enemy && !enemy->getDefeated()) {
            posOfTarget = targetTilePosition;
            if (!collisionHandler.isInCombat() || enemy != collisionHandler.getCurrentTarget()) {
                collisionHandler.startAttackOnEnemy(enemy);
                qDebug() << "movemang attack";
            }
            return 0;
        }

        // Validate the move
        if (isValidMove(targetTile)) {
            bool healthPackCollected = performMove(targetTile);

            if (healthPackCollected) {
                // Signal that the world needs to be rerendered
                emit model.worldChanged();
            }

            // // Check for doors
            // if (targetTile.isNextDoor()) {
            //     qDebug() << "STANDING ON DOOR";
            //     emit model.levelChangeRequested(model.getCurrentLevel()->getLevelIndex() + 1);
            // } else if (targetTile.isPrevDoor()) {
            //     emit model.levelChangeRequested(model.getCurrentLevel()->getLevelIndex() - 1);
            // }
            return 1;
        } else {
            qDebug() << "Attempted move is not valid.";
            return 0;
        }
    } catch (const std::out_of_range& e) {
        QString error_msg = QString("\033[31mError: Attempted to access an out-of-bounds tile. %1.\033[0m").arg(e.what());
        qDebug().noquote() << error_msg;
        return 0;;
    } catch (const std::exception& e) {
        qDebug() << "An unexpected error occurred:" << e.what() << ".";QString error_msg = QString("\033[31mAn unexpected error occurred: %1.\033[0m").arg(e.what());
        qDebug().noquote() << error_msg;
        return 0;
    } catch (...) {
        qDebug().noquote() << "\033[31mAn unknown error occurred while attempting to move.\033[0m";
        return 0;
    }
}

std::pair<int, int> MovementManager::calculateTargetPosition(Direction dir) const {

    // Head the protagonist in the direction of the target position
    model.getCurrentLevel()->getProtagonist()->setProtagonistDirection(dir);

    int currentX = model.getCurrentLevel()->getProtagonist()->getXPos();
    int currentY = model.getCurrentLevel()->getProtagonist()->getYPos();

    int targetX = currentX, targetY = currentY;
    switch (dir) {
    case Direction::UP:    targetY--; break;
    case Direction::DOWN:  targetY++; break;
    case Direction::LEFT:  targetX--; break;
    case Direction::RIGHT: targetX++; break;
    case Direction::UP_LEFT: targetX--; targetY--; break;
    case Direction::UP_RIGHT: targetX++; targetY--; break;
    case Direction::DOWN_LEFT: targetX--; targetY++; break;
    case Direction::DOWN_RIGHT: targetX++; targetY++; break;
    }
    qDebug() << "targetX = " << targetX << "TargetY = " << targetY;
    return {targetX, targetY};
}

bool MovementManager::performMove(TileModel& targetTile) {
    auto protagonist = model.getCurrentLevel()->getProtagonist();
    bool healthPackCollected = false;

    if(hasHealthPack(targetTile)) {
        protagonist->setHealth(protagonist->getHealth()+20);
        healthPackCollected = true;
        // Remove the health pack from the level's collection
        model.getCurrentLevel()->removeHealthPackAt(targetTile.getXPos(), targetTile.getYPos());
    }

    float energyCost = 1 - targetTile.getValue();
    protagonist->setEnergy(protagonist->getEnergy() - energyCost);
    protagonist->setPos(targetTile.getXPos(), targetTile.getYPos());
    qDebug() << "Energy is:" << protagonist->getEnergy();

    return healthPackCollected;
}

bool MovementManager::isValidMove(const TileModel& targetTile) const {
    // TileModel currentTile = model.getTileAt(model.getProtagonist()->getXPos(), model.getProtagonist()->getYPos());
    // if(currentTile.getValue()<targetTile.getValue() + 0.2) {
    //     qDebug() << "Unvalid move.";
    //     return false;
    // }

    float energyCost = 1 - targetTile.getValue();
    float grayValue30 = 30.0f / 255;
    float grayValue128 = 128.0f / 255;

    qDebug() << "Value of tile: " << targetTile.getValue();
    return energyCost < model.getCurrentLevel()->getProtagonist()->getEnergy() &&
           targetTile.getXPos() < model.getCurrentLevel()->getMapCols() &&
           targetTile.getYPos() < model.getCurrentLevel()->getMapRows() &&
           targetTile.getValue() < 1 &&
           targetTile.getValue() > 0 &&
           targetTile.getValue() != grayValue30 &&
           targetTile.getValue() != grayValue128;
}

QString MovementManager::directionToString(Direction dir) const {
    switch (dir) {
    case Direction::UP: return "UP";
    case Direction::DOWN: return "DOWN";
    case Direction::LEFT: return "LEFT";
    case Direction::RIGHT: return "RIGHT";
    default: return "UNKNOWN";
    }
}
