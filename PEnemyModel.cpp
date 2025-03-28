#include "PEnemyModel.h"
#include <cmath>

PEnemyModel::PEnemyModel(int xPos, int yPos, float strength, int poisonRange, float poisonDuration)
    :EnemyModel(xPos, yPos, strength),
    PoisonRange(poisonRange),
    PoisonDuration(poisonDuration) {

}

int PEnemyModel::getPoisonRange(){
    return PoisonRange;
}
