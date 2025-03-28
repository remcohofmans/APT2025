#ifndef GAMEPATHFINDER_H
#define GAMEPATHFINDER_H

#include <vector>
#include "DIRECTION.h"
#include "PathNode.h"
#include <set>


class GameModel;

class GamePathFinder {
public:
    GamePathFinder();

    std::vector<int> findPath(int destX, int destY, bool avoidEnemies);
    std::vector<Direction> retrievePath(const std::vector<int>& path);
    void updateNodes();

private:
    GameModel& model;
    std::vector<PathNode> nodes;
    std::set<std::pair<int, int>> blockedTiles; // Store blocked tiles as (x, y) pairs
    float costFunction(const TileModel& from, const TileModel& to, bool avoidEnemies);
    float heuristicFunction(const TileModel& from, const TileModel& to);
};

#endif // GAMEPATHFINDER_H
