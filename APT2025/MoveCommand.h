#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H

#include "Command.h"
#include "InputController.h"

class MoveCommand : public Command {
public:
    MoveCommand(InputController& controller, Direction dir)
        : controller(controller), direction(dir), name(directionToString(dir)) {}

    void execute(const QStringList& args) override {
        controller.processMovementCommand({name});
    }

    QString getName() const override { return name.toLower(); }
    QString getDescription() const override {
        return QString("Move %1").arg(name.toLower());
    }
    QString getSyntax() const override { return getName(); }

private:
    InputController& controller;
    Direction direction;
    QString name;

    QString directionToString(Direction dir) {
        switch (dir) {
        case Direction::UP: return "UP";
        case Direction::DOWN: return "DOWN";
        case Direction::LEFT: return "LEFT";
        case Direction::RIGHT: return "RIGHT";
        default: return "UNKNOWN";
        }
    }
};

#endif // MOVECOMMAND_H
