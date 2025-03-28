#ifndef TILEMODEL_H
#define TILEMODEL_H

#include "world.h"

class TileModel: public Tile {

private:
    bool poisoned;
    bool nextDoor;
    bool prevDoor;
    float poisonLevel = {0.0};

public:
    TileModel(int xPosition, int yPosition, float tileWeight, bool poisoned);
    explicit TileModel(const Tile& tile);

    bool isPoisoned() const;
    bool isDoor();
    bool isNextDoor();
    bool isPrevDoor();

    void setPoisoned(bool newStatus);
    void setNextDoor();
    void setPrevDoor();
    void setPoisonLevel(int newLevel);
    float getPoisonLevel();
    void decrementPoisonLevel();
    bool isWalkable() const;


    std::pair<int, int> getPos();
};

#endif // TILEMODEL_H
