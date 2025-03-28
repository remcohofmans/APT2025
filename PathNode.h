#ifndef PATHNODE_H
#define PATHNODE_H

#include "TileModel.h"

class PathNode : public TileModel {
public:
    float f = 0;
    float g = 0;
    float h = 0;
    bool visited = false;
    bool closed = false;
    PathNode* prev = nullptr;

    PathNode(const TileModel& tile) : TileModel(tile) {}
};

#endif // PATHNODE_H
