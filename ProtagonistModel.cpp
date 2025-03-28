#include "ProtagonistModel.h"
#include "QDebug"

ProtagonistModel::ProtagonistModel(int xPos, int yPos)
    : TileModel(xPos, yPos, 0.0f, false),
    health(100),
    energy(100),
    poisoned(false),
    status(ProtagonistStatus::IDLE){
}

float ProtagonistModel::getHealth() const {
    return health;
}

float ProtagonistModel::getEnergy() const {
    return energy;
}

ProtagonistStatus ProtagonistModel::getStatus() const {
    return status;
}

int ProtagonistModel::getDamage() const {
    return damage;
}

Direction ProtagonistModel::getDirection() const {
    return protagonistDirection;
}

void ProtagonistModel::setHealth(float newVal) {
    int damage = (int)(getHealth() - newVal);

    newVal = std::clamp( (int)newVal, 0, 100);
    health = newVal;

    if(this->getHealth() == 0){
        this->setStatus(ProtagonistStatus::DEFEATED);
    }
    qDebug() << "Health change emitted!";
    emit healthChanged(damage);
}

void ProtagonistModel::setEnergy(float newVal) {
    energy = newVal;
    emit energyChanged(newVal);
}

void ProtagonistModel::setStatus(ProtagonistStatus newStatus) {
    if(newStatus != status && status != ProtagonistStatus::DEFEATED){
        status = newStatus;
        qDebug() << "Protagonist Status Changed - Emitting Signal:" << static_cast<int>(newStatus);
        emit statusChanged(newStatus);
    }
}

void ProtagonistModel::setProtagonistDirection(Direction newDir){
    protagonistDirection = newDir;
}
