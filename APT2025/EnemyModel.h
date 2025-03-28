#ifndef ENEMYMODEL_H
#define ENEMYMODEL_H

#include "TileModel.h"
#include "ENEMY_STATUS.h"
#include "QObject"

class EnemyModel :public QObject,  public TileModel {
    Q_OBJECT
private:
    float strength;
    bool isDefeated = {false};
    EnemyStatus status;

signals:
    void dead();

public:
    EnemyModel(int xPos, int yPos, float strength);

    bool getDefeated() const;
    EnemyStatus getStatus() const;
    float getStrength() const;

    void setDefeated(bool defeated);
    void setStatus(EnemyStatus newStatus);

    void onDamaged();
};

#endif // ENEMYMODEL_H
