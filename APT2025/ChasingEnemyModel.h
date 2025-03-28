#ifndef CHASINGENEMYMODEL_H
#define CHASINGENEMYMODEL_H

#include "EnemyModel.h"

class ChasingEnemyModel : public EnemyModel {
public:
    ChasingEnemyModel(int xPos, int yPos, float strength, float chaseSpeed);

    float getChaseSpeed() const;
    void setChaseSpeed(float newSpeed);
    bool isAlive();

private:
    float chaseSpeed; // Speed of chasing behavior

};

#endif // CHASINGENEMYMODEL_H
