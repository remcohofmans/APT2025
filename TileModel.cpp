#include "TileModel.h"
#include "QDebug"

TileModel::TileModel(int xPosition, int yPosition, float tileWeight, bool poisoned):
    Tile(xPosition, yPosition, tileWeight),
    poisoned(poisoned),
    nextDoor(false),
    prevDoor(false)
    {
    //qDebug() << "TileModel Basic Ctor";
}

TileModel::TileModel(const Tile& tile)
    : Tile(tile),
    poisoned(false),
    nextDoor(false),
    prevDoor(false)
    {
}

std::pair<int, int> TileModel::getPos(){
    return {this->xPos, this->yPos};
}

bool TileModel::isPoisoned()const {
    return poisoned;
}

bool TileModel::isNextDoor(){
    return nextDoor;
}

bool TileModel::isPrevDoor(){
    return prevDoor;
}

bool TileModel::isDoor(){
    return isPrevDoor() || isNextDoor();
}

void TileModel::setNextDoor(){
    nextDoor = true;
}

void TileModel::setPrevDoor(){
    prevDoor = true;
}

void TileModel::setPoisoned(bool newStatus){
    qDebug() << "Poisoning tile" << this->getPos();
    poisoned = newStatus;
}

void TileModel::setPoisonLevel(int level){
    poisonLevel = level;
}

float TileModel::getPoisonLevel(){
    return poisonLevel;
}

void TileModel::decrementPoisonLevel(){
    poisonLevel = poisonLevel - 0.2;
}

bool TileModel::isWalkable() const {
    float grayValue30 = 30.0f / 255;
    float grayValue128 = 128.0f / 255;
    return value > 0 && value < 1 && value != grayValue30 && value != grayValue128;
}
