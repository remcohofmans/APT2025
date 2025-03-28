#include "Level.h"
#include "qtimer.h"
#include "world.h"
#include "QDebug"
#include "ENEMY_STATUS.h"
#include "world.h"
#include "PEnemyModel.h"
#include <queue>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <utility>

Level::Level(int levelIndex) : level(levelIndex), chasingEnemies {} {
    //qDebug() << "Level created";
    startPoisonDecrementTimer();
    //startChasingEnemyTimer();
}

void Level::createWorld(const QString &worldPath, const QString &overlayPath, int levelIndex){

    std::unique_ptr<World> world = std::make_unique<World>(); //maybe make the enemies and healthpacks a variable so they can change based on the level
    world->createWorld(worldPath, 10, 5, 0.1f);
    int i = 0;

    for (auto& enemy : world->getEnemies()) {

        // Check if the enemy is a PEnemy
        //qDebug() << "Processing enemy at position:" << enemy->getXPos() << enemy->getYPos();
        qDebug() << typeid(enemy).name();
        PEnemy* poisonEnemy = dynamic_cast<PEnemy*>(enemy.get());
        std::shared_ptr<EnemyModel> enemyModel;
        //qDebug() << enemy->getXPos() << enemy->getYPos();
        //qDebug() << typeid(poisonEnemy).name();

        if
            //checks if the enemies are poison or not currently on false for the chasing enemy
            (poisonEnemy) {
            //qDebug() << "PEnemyModel created";
            enemyModel = std::make_shared<PEnemyModel>(poisonEnemy->getXPos(), poisonEnemy->getYPos(), poisonEnemy->getValue(), 5, 1.0f);
            auto poisonEnemyModel = std::dynamic_pointer_cast<PEnemyModel>(enemyModel);
            QObject::connect(poisonEnemyModel.get(), &PEnemyModel::poisonSurroundingTiles, this, &Level::HandlePoisonSurroundingTiles);
        }else {
            // Convert enemies to ChasingEnemyModel based on a condition now 1 is converted
            if (i < 1) { // Example condition probably would make a parameter for this in the createworld FUNC
                i++;
                //Creates a new ChasingEnemyModel in enemyModel (wich is an EnemyModel)
                enemyModel = std::make_shared<ChasingEnemyModel>(enemy->getXPos(), enemy->getYPos(), enemy->getValue(), 5.0f);
                // Adds enemyModel to the chasingEnemies, but first casts it to a ChasingEnemyModel bc, chasingEnmeis requires a ChasingEnemy
                chasingEnemies.push_back(std::dynamic_pointer_cast<ChasingEnemyModel>(enemyModel));

            } else { //makes normal enemies of the rest of the remaining enemies
                enemyModel = std::make_shared<EnemyModel>(enemy->getXPos(), enemy->getYPos(), enemy->getValue());
            }
        }

        enemyModel->setStatus(EnemyStatus::IDLE);
        enemyModels.push_back(enemyModel);
    }


    for (auto &tile : world->getTiles()) {
        tileModels.push_back(std::make_shared<TileModel>(*(std::move(tile)) ) );
    }

    for (auto &healthPack : world->getHealthPacks()) {
        healthPackModels.push_back(std::make_shared<TileModel>(*(std::move(healthPack)) ) );
    }

    overlay = overlayPath;
    level = levelIndex;

    this->mapCols = world->getCols();
    this->mapRows = world->getRows();

    std::unique_ptr<Protagonist> worldProtagonist = world->getProtagonist();
    if(worldProtagonist){
        protagonistModel = std::make_shared<ProtagonistModel>(
            worldProtagonist->getXPos(),
            worldProtagonist->getYPos()
        );
    }
    //qDebug() << "Level Created With Index:" << levelIndex;
}

void Level::setNextDoor(int x, int y){
    getTileAt(x,y).setNextDoor();
}

void Level::setPrevDoor(int x, int y){
    getTileAt(x,y).setPrevDoor();
}

void Level::HandlePoisonSurroundingTiles(const std::pair<int, int>& enemyPosition, int poisonRange, float poisonDuration) {
    qDebug() << "In handlePoisonTiles";
    for (const auto& tile : tileModels) {
        std::pair<int, int> tilePosition = tile->getPos();

        int distance = std::abs(tilePosition.first - enemyPosition.first) + std::abs(tilePosition.second - enemyPosition.second);

        if (distance <= poisonRange && !tile->isPoisoned()) {
            tile->setPoisoned(true);
            tile->setPoisonLevel(poisonDuration);
            qDebug() << "PoisonLevel: " << tile->getPoisonLevel();
        }
    }
    emit poisonStatusChanged();
}

void Level::decrementAllPoisonLevels() {
    for (auto& tile : tileModels) {
        if (tile->isPoisoned()) {
            tile->decrementPoisonLevel();

            if (tile->getPoisonLevel() <= 0) {
                tile->setPoisoned(false);
            }
            emit poisonStatusChanged();
        }
    }
}

void Level::checkProtagonistPoisonDamage(){
    auto pos = protagonistModel->getPos();
    if(getTileAt(pos.first, pos.second).isPoisoned()){
        protagonistModel->setHealth(protagonistModel->getHealth() - (getTileAt(pos.first, pos.second).getPoisonLevel()*10) );
    }
}

void Level::startPoisonDecrementTimer() {
    QTimer* poisonDecrementTimer = new QTimer(this);
    connect(poisonDecrementTimer, &QTimer::timeout, this, &Level::decrementAllPoisonLevels);
    connect(poisonDecrementTimer, &QTimer::timeout, this, &Level::checkProtagonistPoisonDamage);
    poisonDecrementTimer->start(1000);
}

//gets called by the timer and calls the move function.
void Level::updateChasingEnemies() {
    qDebug() << "Updating chasing enemies...";

    for (auto& enemy : chasingEnemies) {
        if (enemy->isAlive()){
            auto [protagonistX, protagonistY] = protagonistModel->getPos();

            moveTowards(protagonistX, protagonistY, *enemy);
            emit enemyMoved();
            if (inAttackRange()) {
                protagonistModel->setHealth(protagonistModel->getHealth()-20);
             return;
            }
        }
        else{
            chasingEnemyTimer->stop();
            delete chasingEnemyTimer;
        }
    }
}

//creates a timer that calls the updateChasingEnemies function every second
void Level::startChasingEnemyTimer() {
    chasingEnemyTimer = new QTimer(this); // The parent ensures cleanup
    connect(chasingEnemyTimer, &QTimer::timeout, this, &Level::updateChasingEnemies);
    chasingEnemyTimer->start(1000);
}

//Function called by the updateChasing Enemies, this will move the enemy towards the current position of the protagonist, would be nice if we can inplementate the pathfinder here
void Level::moveTowards(int targetX, int targetY, ChasingEnemyModel& enemy) {
    struct Node {
        int x, y;
        int f, g, h;
        Node(int x, int y) : x(x), y(y), f(0), g(0), h(0) {}
        bool operator>(const Node& other) const {
            return f > other.f;
        }
    };

    auto enemyPosition = enemy.getPos();
    int startX = enemyPosition.first;
    int startY = enemyPosition.second;

    // Get world size
    int width = sqrt(tileModels.size());

    // Check if we're already at the target
    if (startX == targetX && startY == targetY) {
        return;
    }

    // Track the last position to avoid backtracking
    static std::pair<int, int> lastPosition = {-1, -1};

    // Priority queue for A* algorithm
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<int, bool> closedSet;
    std::unordered_map<int, std::pair<int, int>> cameFrom;

    Node start(startX, startY);
    start.h = std::abs(startX - targetX) + std::abs(startY - targetY);
    start.f = start.h;
    openSet.push(start);

    // Movement directions (up, right, down, left)
    const int dx[] = {0, 1, 0, -1};
    const int dy[] = {-1, 0, 1, 0};

    bool pathFound = false;
    Node current(0, 0);

    while (!openSet.empty()) {
        current = openSet.top();
        openSet.pop();

        // Check if the node is the target
        if (current.x == targetX && current.y == targetY) {
            pathFound = true;
            break;
        }

        // Mark the current node as closed
        closedSet[current.y * width + current.x] = true;

        for (int i = 0; i < 4; i++) {
            int newX = current.x + dx[i];
            int newY = current.y + dy[i];

            // Skip if this is the last position to avoid oscillation
            if (newX == lastPosition.first && newY == lastPosition.second) {
                continue;
            }

            // Check boundaries
            if (newX < 0 || newX >= width || newY < 0 || newY >= width) {
                continue;
            }

            // Skip if position is closed or tile is unwalkable
            try {
                TileModel& tile = getTileAt(newX, newY);
                if (!tile.isWalkable() || closedSet[newY * width + newX]) {
                    continue;
                }
            } catch (const std::out_of_range&) {
                continue;
            }

            Node neighbor(newX, newY);
            int tentativeG = current.g + 1;

            // Calculate costs
            neighbor.g = tentativeG;
            neighbor.h = std::abs(newX - targetX) + std::abs(newY - targetY);
            neighbor.f = neighbor.g + neighbor.h;

            // Track the path
            cameFrom[newY * width + newX] = {current.x, current.y};
            openSet.push(neighbor);
        }
    }

    // If we found a path, move one step along it
    if (pathFound) {
        int currentX = targetX;
        int currentY = targetY;

        // Retrace the path
        while (cameFrom[currentY * width + currentX] != std::make_pair(startX, startY)) {
            auto previous = cameFrom[currentY * width + currentX];
            currentX = previous.first;
            currentY = previous.second;
        }

        // Update the last position and move
        if (!inAttackRange()){
        lastPosition = {startX, startY};
        enemy.setXPos(currentX);
        enemy.setYPos(currentY);
        qDebug() << "Enemy moved to:" << currentX << "," << currentY;
        }
    } else {
        qDebug() << "No path found to target";
    }
}



TileModel& Level::getTileAt(int xPos, int yPos) const{
    int width = sqrt(tileModels.size());
    int index = xPos + width * yPos;

    if (xPos >= 0 && xPos < width && yPos >= 0 && yPos < width && index < (int)tileModels.size()) {
        return *(tileModels[index]);
    }
    throw std::out_of_range("Invalid position");
}

void Level::removeHealthPackAt(int x, int y) {
    protagonistModel->setHealing(true);
    qDebug() << "No. of health packs before:" << healthPackModels.size();
    auto it = std::remove_if(healthPackModels.begin(), healthPackModels.end(),
                             [x, y](const std::shared_ptr<TileModel>& pack) {
                                 return pack->getXPos() == x && pack->getYPos() == y;
                             });
    healthPackModels.erase(it, healthPackModels.end());
    qDebug() << "HealthPack removed from scene.";
    qDebug() << "No. of health packs after:" << healthPackModels.size();
}
//refactor zodat dit extendeble is voor incase dat er meer enemyies zouden in chasingenmies ziten maar voor nu gewoon op de eerste want er is toch maar 1
bool Level::inAttackRange(){

    if(
        (chasingEnemies[0]->getXPos() - 1 == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() -1 == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() -1 == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() + 1 == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() -1 == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() - 1 == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() + 1 == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() - 1 == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() +1 == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() +1 == protagonistModel->getYPos())  ||
        (chasingEnemies[0]->getXPos() + 1 == protagonistModel->getXPos()  && chasingEnemies[0]->getYPos() +1 == protagonistModel->getYPos()) ){
        qDebug() << "inRange";
        return true;
    }
    else{
        return false;
    }   
}
