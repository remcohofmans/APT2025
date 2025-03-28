#include "CollisionHandler.h"
#include "qdebug.h"
#include "PEnemyModel.h"
#include "GameModel.h"

CollisionHandler::CollisionHandler(QObject* parent) : QObject(parent), model(GameModel::getInstance()), currentTarget(nullptr), combatTimer(new QTimer(this)) {
    connect(combatTimer, &QTimer::timeout, this, &CollisionHandler::processCombatTick);
}


bool CollisionHandler::isEnemyAt(const TileModel& targetTile) {
    return findEnemyAt(targetTile) != nullptr;
}

void CollisionHandler::startAttackOnEnemy(std::shared_ptr<EnemyModel> enemy) {
    if(model.getCurrentLevel()->getProtagonist()->getStatus() == ProtagonistStatus::DEFEATED){
        return;
    }
    if (currentTarget == enemy && combatTimer->isActive()) {
        qDebug() << "Already attacking this enemy.";
        return;
    }

    if (enemy && !enemy->getDefeated()) {
        currentTarget = enemy;
        model.getCurrentLevel()->getProtagonist()->setStatus(ProtagonistStatus::ATTACKING);
        combatTimer->start(1000);
        qDebug() << "Starting attack";
    }
}

bool CollisionHandler::isInCombat()const {
    return currentTarget != nullptr && !currentTarget->getDefeated();
}

std::shared_ptr<EnemyModel> CollisionHandler::findEnemyAt(const TileModel& tileToCheck) {
    const QVector<std::shared_ptr<EnemyModel>>& enemies = model.getCurrentLevel()->getEnemies();
    int targetX = tileToCheck.getXPos();
    int targetY = tileToCheck.getYPos();

    for (const auto& enemy : enemies) {
        if (enemy && enemy->getXPos() == targetX && enemy->getYPos() == targetY) {
            return enemy;
        }
    }
    return nullptr;
}

std::shared_ptr<EnemyModel> CollisionHandler::getCurrentTarget() const {
    return currentTarget;
}

void CollisionHandler::stopAttacking() {
    if (currentTarget) {
        currentTarget = nullptr;
        model.getCurrentLevel()->getProtagonist()->setStatus(ProtagonistStatus::IDLE);
        combatTimer->stop();
        qDebug() << "Attack stopped";
        emit combatEnded();
    }
}

void CollisionHandler::processCombatTick() {
    if(model.getCurrentLevel()->getProtagonist()->getStatus() == ProtagonistStatus::DEFEATED){
        stopAttacking();
    }
    if (currentTarget && !currentTarget->getDefeated()) {
        int damage = model.getCurrentLevel()->getProtagonist()->getDamage();
        currentTarget->setValue(currentTarget->getValue() - damage);
        model.getCurrentLevel()->getProtagonist()->setHealth(model.getCurrentLevel()->getProtagonist()->getHealth() - currentTarget->getStrength());
        qDebug() << "Attack";
        PEnemyModel* poisonEnemy = dynamic_cast<PEnemyModel*>(currentTarget.get());
        if(poisonEnemy){
            emit poisonEnemy->poisonSurroundingTiles(poisonEnemy->getPos(), poisonEnemy->getPoisonRange(), 1.0f);
        }

        if (currentTarget->getValue() <= 0) {
            currentTarget->setDefeated(true);
            emit currentTarget->dead();
            stopAttacking();
        }
    }
}
