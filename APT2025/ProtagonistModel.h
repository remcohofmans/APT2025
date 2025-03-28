#ifndef PROTAGONISTMODEL_H
#define PROTAGONISTMODEL_H

#include "PROTAGONIST_STATUS.h"
#include "TileModel.h"
#include "QObject"
#include "DIRECTION.h"

class ProtagonistModel: public QObject, public TileModel {
    Q_OBJECT
private:
    float health;
    float energy;
    bool poisoned;
    ProtagonistStatus status;
    int damage = {10};
    Direction protagonistDirection = Direction::DOWN;
    bool isHealing = false;

signals:
    void posChanged(int x, int y);
    void healthChanged(int damage);
    void energyChanged(int e);
    void statusChanged(ProtagonistStatus newStatus);

public:
    explicit ProtagonistModel(int xPos, int yPos);

    float getHealth() const;
    float getEnergy() const;
    ProtagonistStatus getStatus() const;
    int getDamage() const;
    Direction getDirection() const;
    bool getHealing() const { return isHealing; }

    void setHealing(bool healing) { isHealing = healing; }

    void setXPos(int newPos) noexcept override {if (xPos != newPos){xPos = newPos; emit posChanged(xPos, yPos);}}
    void setYPos(int newPos) noexcept override {if (yPos != newPos){yPos = newPos; emit posChanged(xPos, yPos);}}
    void setPos(int newX, int newY) noexcept {if (xPos != newX || yPos != newY) {xPos = newX; yPos = newY; emit posChanged(xPos, yPos);}}

    void setHealth(float newVal);
    void setEnergy(float newVal);
    void setStatus(ProtagonistStatus newStatus);
    void setDamage(int value);
    void setProtagonistDirection(Direction newDir);
};

#endif // PROTAGONISTMODEL_H
