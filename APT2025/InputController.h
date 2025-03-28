#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QWidget>
#include <QTimer>
#include <QKeyEvent>
#include <QMap>
#include <memory>
#include "GameModel.h"
#include "MovementManager.h"
#include "GamePathFinder.h"
#include "DIRECTION.h"

class InputController : public QWidget {
    Q_OBJECT

public:
    explicit InputController(QWidget* parent = nullptr);
    virtual ~InputController();

    void handleCommand(const QString& textInput);
    void handleTileClick(int x, int y);

    // Command processing
    void processGotoCommand(const QStringList& commands);
    void processMovementCommand(const QStringList& commands);

    // Emergency handling
    void seekNearestHealthPack();

signals:
    void protagonistPosUpdated();
    void pathUpdated(const std::vector<std::pair<int, int>>& path);

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    // Core game components
    GameModel& model;
    MovementManager movementManager;
    std::unique_ptr<GamePathFinder> pathFinder;
    QTimer* pathfindingTimer;

    // Pathfinding state
    std::vector<Direction> currentPath;
    int currentPathIndex;
    bool isPathfindingActive;
    bool isEmergencyMode;
    std::pair<int, int> originalDestination;
    std::pair<int, int> targetDestination;
    bool wasPathfindingActive;  // Flag to track if we were pathfinding before emergency 

    // Pathfinding methods
    void startPathfinding(int destX, int destY, bool avoidEnemies);
    void stopPathfinding();
    void recalculatePath();
    std::vector<std::pair<int, int>> calculatePathTiles();
    bool validateDestination(int destX, int destY);

    // Emergency handling
    void checkEmergencyMode();
    void resumeOriginalPath();

    void initializeCommands();

private slots:
    void continuePathfinding();
    void onCombatEnded();
};

#endif // INPUTCONTROLLER_H
