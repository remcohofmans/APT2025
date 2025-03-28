#include "GamePathFinder.h"
#include "pathfinder_class.h"
#include "GameModel.h"
#include "PathNode.h"
#include <cmath>
#include <limits>
#include <QDebug>


GamePathFinder::GamePathFinder()
    : model(GameModel::getInstance())  // Initialize the model reference
{
    auto currentLevel = model.getCurrentLevel();

    // Create nodes vector
    for (int y = 0; y < currentLevel->getMapRows(); y++) {
        for (int x = 0; x < currentLevel->getMapCols(); x++) {
            nodes.emplace_back(currentLevel->getTileAt(x, y));
        }
    }
}

// Define cost function
float GamePathFinder::costFunction(const TileModel& from, const TileModel& to, bool avoidEnemies) {
    float grayValue30 = 30.0f / 255.0f;
    float grayValue128 = 128.0f / 255.0f;
    float value = to.getValue();

    // Check for impassable tile conditions - these are always impassable regardless of mode
    if (value == grayValue30 || value == grayValue128 || value >= 1.0f || value <= 0.0f) {
        return std::numeric_limits<float>::infinity();
    }

    float cost = 1.0f - value;  // Base cost

    if (avoidEnemies) {
        // In emergency mode, check for enemies and add high cost to enemy tiles
        auto enemies = model.getCurrentLevel()->getEnemies();
        for (const auto& enemy : enemies) {
            if (enemy->getXPos() == to.getXPos() && enemy->getYPos() == to.getYPos()) {
                return std::numeric_limits<float>::infinity();  // Make enemy tiles impassable in emergency mode
            }
        }

        // Also add higher cost to tiles near enemies
        for (const auto& enemy : enemies) {
            int distanceToEnemy = std::abs(enemy->getXPos() - to.getXPos()) +
                                  std::abs(enemy->getYPos() - to.getYPos());
            if (distanceToEnemy <= 2) {  // Within 2 tiles of enemy
                cost += (3 - distanceToEnemy) * 5.0f;  // Higher cost closer to enemies
            }
        }
    }

    return cost;
}

// Define heuristic function
float GamePathFinder::heuristicFunction(const TileModel& from, const TileModel& to) {
    int dx = std::abs(from.getXPos() - to.getXPos());
    int dy = std::abs(from.getYPos() - to.getYPos());
    return std::sqrt(dx * dx + dy * dy);
}

// Find the path using A* algorithm
std::vector<int> GamePathFinder::findPath(int destX, int destY, bool avoidEnemies) {
    updateNodes(); // Ensure the nodes are reset

    auto currentLevel = model.getCurrentLevel();
    auto protagonist = currentLevel->getProtagonist();
    TileModel& startTile = currentLevel->getTileAt(protagonist->getXPos(), protagonist->getYPos());
    TileModel& destTile = currentLevel->getTileAt(destX, destY);

    // Setup comparator for priority queue
    Comparator<PathNode> comparator = [](const PathNode& a, const PathNode& b) {
        return a.f > b.f;
    };

    // Create pathfinder instance
    PathFinder<PathNode, TileModel> pathFinder(
        nodes,
        &startTile,
        &destTile,
        comparator,
        currentLevel->getMapCols(),
        [this, avoidEnemies](const TileModel& from, const TileModel& to) {
            return costFunction(from, to, avoidEnemies);
        },
        [this](const TileModel& from, const TileModel& to) {
            return heuristicFunction(from, to);
        },
        0.0f  // heuristic weight
        );

    // Find path
    return pathFinder.A_star();
}

// Execute the path
std::vector<Direction> GamePathFinder::retrievePath(const std::vector<int>& path) {
    std::vector<Direction> moveList;  // This container will hold the directions of the moves

    // Map the path indices to actual directions
    for (int direction : path) {
        Direction moveDir;
        switch (direction) {
        case 0: moveDir = Direction::UP; break;
        case 1: moveDir = Direction::UP_RIGHT; break;
        case 2: moveDir = Direction::RIGHT; break;
        case 3: moveDir = Direction::DOWN_RIGHT; break;
        case 4: moveDir = Direction::DOWN; break;
        case 5: moveDir = Direction::DOWN_LEFT; break;
        case 6: moveDir = Direction::LEFT; break;
        case 7: moveDir = Direction::UP_LEFT; break;
        default:
            qDebug() << "\033[31mUnexpected direction\033[0m";
            continue;
        }
        moveList.push_back(moveDir);  // Add the move direction to the list
    }

    return moveList;  // Return the list of directions
}

void GamePathFinder::updateNodes() {
    nodes.clear();
    auto currentLevel = model.getCurrentLevel();

    for (int y = 0; y < currentLevel->getMapRows(); y++) {
        for (int x = 0; x < currentLevel->getMapCols(); x++) {
            nodes.emplace_back(currentLevel->getTileAt(x, y));
        }
    }
}


