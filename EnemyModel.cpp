#include "EnemyModel.h"
#include "QDebug"

EnemyModel::EnemyModel(int xPos, int yPos, float strength)
    : TileModel(xPos, yPos, strength, false)
    , strength(10.0f)
    , isDefeated(false)
    , status(EnemyStatus::IDLE) {
    //qDebug() << "EnemyModel Ctor Called";
}

bool EnemyModel::getDefeated() const {
    return isDefeated;
}

EnemyStatus EnemyModel::getStatus() const {
    return status;
}

float EnemyModel::getStrength() const {
    return strength;
}

void EnemyModel::setDefeated(bool defeated) {
    isDefeated = defeated;
}

void EnemyModel::setStatus(EnemyStatus newStatus) {
    status = newStatus;
}
