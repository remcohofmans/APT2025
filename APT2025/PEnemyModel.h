#ifndef PENEMYMODEL_H
#define PENEMYMODEL_H

#include "EnemyModel.h"

class PEnemyModel : public EnemyModel {
    Q_OBJECT
public:
    PEnemyModel(int xPos, int yPos, float strength, int poisonRange, float poisonDuration);
    int getPoisonRange();

    // virtual void OnDamaged() override;

signals:
    void poisonSurroundingTiles(const std::pair<int, int>& enemyPosition, int poisonRange, float poisonDuration);

private:
    int PoisonRange;
    float PoisonDuration = {1.0f};

};

#endif // PENEMYMODEL_H
