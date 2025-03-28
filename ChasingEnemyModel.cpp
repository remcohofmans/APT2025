#include "ChasingEnemyModel.h"
#include <QDebug>

ChasingEnemyModel::ChasingEnemyModel(int xPos, int yPos, float strength, float chaseSpeed)
    : EnemyModel(xPos, yPos, strength) // Call base class constructor
    , chaseSpeed(chaseSpeed) {
    //qDebug() << "ChasingEnemyModel Ctor Called";
}

float ChasingEnemyModel::getChaseSpeed() const {
    return chaseSpeed;
}

void ChasingEnemyModel::setChaseSpeed(float newSpeed) {
    chaseSpeed = newSpeed;
}

bool ChasingEnemyModel::isAlive(){
    return (this->getValue() >0);
}
