#include "InputController.h"
#include "AttackCommand.h"
#include "CommandRegistry.h"
#include "GoToCommand.h"
#include "HealCommand.h"
#include "HelpCommand.h"
#include "MoveCommand.h"
#include <QDebug>

InputController::InputController(QWidget* parent)
    : QWidget(parent)
    , model(GameModel::getInstance())
    , movementManager()
    , pathFinder(std::make_unique<GamePathFinder>())
    , pathfindingTimer(new QTimer(this))
    , currentPathIndex(0)
    , isPathfindingActive(false)
    , isEmergencyMode(false)
    , originalDestination({-1, -1})
    , targetDestination({-1, -1})
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    initializeCommands();

    // Setup timer for pathfinding attempts
    connect(pathfindingTimer, &QTimer::timeout,
            this, &InputController::continuePathfinding);

    // Connect to combat signals
    connect(&movementManager.getCollisionHandler(), &CollisionHandler::combatEnded,
            this, &InputController::onCombatEnded);
}

InputController::~InputController() {
    stopPathfinding();
}

void InputController::keyPressEvent(QKeyEvent* event) {
    if (isEmergencyMode || !event) {
        return; // Ignore manual movement when in emergency mode
    }

    static const QMap<int, Direction> keyToDirection = {
        {Qt::Key_W, Direction::UP},
        {Qt::Key_Up, Direction::UP},
        {Qt::Key_A, Direction::LEFT},
        {Qt::Key_Left, Direction::LEFT},
        {Qt::Key_S, Direction::DOWN},
        {Qt::Key_Down, Direction::DOWN},
        {Qt::Key_D, Direction::RIGHT},
        {Qt::Key_Right, Direction::RIGHT}
    };

    if (keyToDirection.contains(event->key())) {
        stopPathfinding();

        auto it = keyToDirection.find(event->key());
        auto protagonist = model.getCurrentLevel()->getProtagonist();
        if (protagonist) {
            protagonist->setProtagonistDirection(it.value());
            if (movementManager.attemptMove(it.value())) {
                emit protagonistPosUpdated();
            }
            qDebug() << "Moving" << movementManager.directionToString(it.value());
        }
    }
}

void InputController::handleCommand(const QString& textInput) {
    if (textInput.isEmpty()) return;

    if (!CommandRegistry::getInstance().executeCommand(textInput)) {
        qDebug() << "Unknown or ambiguous command:" << textInput;
    }
}

void InputController::handleTileClick(int x, int y) {
    qDebug() << "Called tile click handler";
    processGotoCommand(QStringList{"GOTO", QString::number(x), QString::number(y)});
    setFocus();
}

void InputController::processMovementCommand(const QStringList& commands) {
    static const QMap<QString, Direction> stringToDirection = {
        {"UP", Direction::UP},
        {"DOWN", Direction::DOWN},
        {"LEFT", Direction::LEFT},
        {"RIGHT", Direction::RIGHT}
    };

    QString direction = commands[1].toUpper();
    auto it = stringToDirection.find(direction);
    if (it != stringToDirection.end()) {
        stopPathfinding();  // Stop any active pathfinding
        if (movementManager.attemptMove(it.value())) {
            emit protagonistPosUpdated();
        }
    } else {
        qDebug() << "Invalid direction:" << direction;
    }
}

bool InputController::validateDestination(int destX, int destY) {
    auto currentLevel = model.getCurrentLevel();
    if (!currentLevel) return false;

    return (destX >= 0 && destY >= 0 &&
            destX < currentLevel->getMapCols() &&
            destY < currentLevel->getMapRows());
}

void InputController::processGotoCommand(const QStringList& commands) {
    bool ok1, ok2;
    int destX = commands[1].toInt(&ok1);
    int destY = commands[2].toInt(&ok2);

    if (!ok1 || !ok2) {
        qDebug() << "\033[31mInvalid coordinates provided\033[0m";
        return;
    }

    if (!validateDestination(destX, destY)) {
        qDebug() << "\033[31mDestination coordinates out of bounds\033[0m";
        return;
    }

    stopPathfinding();
    startPathfinding(destX, destY, false);
}

void InputController::startPathfinding(int destX, int destY, bool avoidEnemies) {

    targetDestination = {destX, destY};
    isPathfindingActive = true;
    currentPathIndex = 0;

    std::vector<int> path = pathFinder->findPath(destX, destY, avoidEnemies);
    qDebug() << path;
    currentPath = pathFinder->retrievePath(path);

    if (currentPath.empty()) {
        qDebug() << "No valid path found";
        stopPathfinding();
        return;
    }

    // Update path visualization
    std::vector<std::pair<int, int>> pathTiles = calculatePathTiles();
    emit pathUpdated(pathTiles);

    pathfindingTimer->start(100);
}

std::vector<std::pair<int, int>> InputController::calculatePathTiles() {
    std::vector<std::pair<int, int>> pathTiles;
    auto protagonist = model.getCurrentLevel()->getProtagonist();
    if (!protagonist) return pathTiles;

    int currentX = protagonist->getXPos();
    int currentY = protagonist->getYPos();
    pathTiles.push_back({currentX, currentY});

    for (const Direction& dir : currentPath) {
        switch (dir) {
        case Direction::UP: currentY--; break;
        case Direction::DOWN: currentY++; break;
        case Direction::LEFT: currentX--; break;
        case Direction::RIGHT: currentX++; break;
        case Direction::UP_LEFT: currentX--; currentY--; break;
        case Direction::UP_RIGHT: currentX++; currentY--; break;
        case Direction::DOWN_LEFT: currentX--; currentY++; break;
        case Direction::DOWN_RIGHT: currentX++; currentY++; break;
        }
        pathTiles.push_back({currentX, currentY});
    }
    return pathTiles;
}

void InputController::continuePathfinding() {
    checkEmergencyMode();

    if (currentPathIndex >= (int)currentPath.size()) {
        stopPathfinding();
        if (isEmergencyMode) {
            seekNearestHealthPack();
        }
        return;
    }

    Direction nextMove = currentPath[currentPathIndex];
    int moveResult = movementManager.attemptMove(nextMove);

    if (moveResult == 1) {  // Move successful
        emit protagonistPosUpdated();
        currentPathIndex++;
    } else if (moveResult == 0) {  // Move blocked
        if (movementManager.getCollisionHandler().isInCombat()) {
            qDebug() << "Combat encountered - pausing pathfinding";
        } else {
            qDebug() << "Path blocked - recalculating";
            recalculatePath();
        }
    }
}

void InputController::recalculatePath() {
    auto protagonist = model.getCurrentLevel()->getProtagonist();
    if (!protagonist) return;

    qDebug() << "Recalculating path from current position"
             << protagonist->getXPos() << "," << protagonist->getYPos()
             << "to:" << targetDestination.first << "," << targetDestination.second;

    pathFinder->updateNodes();  // Ensure nodes are up to date

    std::vector<int> newPath = pathFinder->findPath(
        targetDestination.first,
        targetDestination.second,
        isEmergencyMode  // Avoid enemies in emergency mode
        );

    currentPath = pathFinder->retrievePath(newPath);
    currentPathIndex = 0;  // Reset path index since this is a new path

    if (currentPath.empty()) {
        qDebug() << "No valid path found after recalculation";
        stopPathfinding();
    } else {
        // Update path visualization after recalculation
        std::vector<std::pair<int, int>> pathTiles = calculatePathTiles();
        emit pathUpdated(pathTiles);
    }
}

void InputController::stopPathfinding() {
    if (isPathfindingActive) {
        pathfindingTimer->stop();
        isPathfindingActive = false;
        currentPath.clear();
        currentPathIndex = 0;
        pathFinder->updateNodes();
        qDebug() << "Pathfinding stopped";
    }
}

void InputController::onCombatEnded() {
    if (isPathfindingActive) {
        qDebug() << "Combat ended - recalculating path";
        pathFinder->updateNodes();
        recalculatePath();
    }
}

void InputController::checkEmergencyMode() {
    auto protagonist = model.getCurrentLevel()->getProtagonist();
    if (!protagonist) return;

    // Check if health is at or below emergency threshold
    if (protagonist->getHealth() <= 20) {
        if (!isEmergencyMode) {
            isEmergencyMode = true;
            wasPathfindingActive = isPathfindingActive;  // Store pathfinding state

            // Store original destination if we were pathfinding
            if (isPathfindingActive) {
                originalDestination = targetDestination;
            }

            // Immediately seek health pack
            seekNearestHealthPack();
        }
    } else if (protagonist->getHealth() > 20 && isEmergencyMode) {
        // Health restored, exit emergency mode
        isEmergencyMode = false;

        // Only resume original path if we were pathfinding before emergency
        if (wasPathfindingActive) {
            resumeOriginalPath();
        }
    }
}

void InputController::seekNearestHealthPack() {

    auto level = model.getCurrentLevel();
    if (!level) return;

    const auto& healthPacks = level->getHealthPacks();
    if (healthPacks.empty()) {
        qDebug() << "No health packs available!";
        return;
    }

    auto protagonist = level->getProtagonist();
    if (!protagonist) return;

    // Find nearest reachable health pack
    int minDistance = INT_MAX;
    int nearestX = -1;
    int nearestY = -1;
    int protX = protagonist->getXPos();
    int protY = protagonist->getYPos();

    pathFinder->updateNodes();  // Ensure pathfinding grid is up to date

    for (const auto& healthPack : healthPacks) {
        int healthX = healthPack->getXPos();
        int healthY = healthPack->getYPos();

        // Calculate path to this health pack (with enemy avoidance)
        std::vector<int> testPath = pathFinder->findPath(healthX, healthY, true);

        if (!testPath.empty()) {
            // Use Manhattan distance for quick distance calculation
            int distance = std::abs(healthX - protX) + std::abs(healthY - protY);
            if (distance < minDistance) {
                minDistance = distance;
                nearestX = healthX;
                nearestY = healthY;
            }
        }
    }

    if (nearestX != -1 && nearestY != -1) {
        // Stop current pathfinding and start pathfinding to health pack
        stopPathfinding();
        startPathfinding(nearestX, nearestY, true);  // true = avoid enemies
        qDebug() << "Emergency: Pathfinding to health pack at" << nearestX << "," << nearestY;
    } else {
        qDebug() << "No reachable health packs found!";
    }
}

void InputController::resumeOriginalPath() {
    if (originalDestination.first != -1 && originalDestination.second != -1) {
        qDebug() << "Resuming path to original destination:"
                 << originalDestination.first << "," << originalDestination.second;

        // Reset emergency-related flags
        isEmergencyMode = false;
        wasPathfindingActive = false;

        // Start pathfinding to original destination
        startPathfinding(originalDestination.first, originalDestination.second, false);

        // Reset stored destination
        originalDestination = {-1, -1};
    }
}

void InputController::initializeCommands() {
    auto& registry = CommandRegistry::getInstance();

    // Register commands
    registry.registerCommand(std::make_unique<GotoCommand>(*this));
    registry.registerCommand(std::make_unique<AttackCommand>(*this));
    registry.registerCommand(std::make_unique<HealCommand>(*this));
    registry.registerCommand(std::make_unique<HelpCommand>());
}

