#ifndef HEALCOMMAND_H
#define HEALCOMMAND_H

#include "Command.h"
#include "InputController.h"

class HealCommand : public Command {
public:
    HealCommand(InputController& controller) : controller(controller) {}

    void execute(const QStringList& args) override {
        controller.seekNearestHealthPack();
    }

    QString getName() const override { return "heal"; }
    QString getDescription() const override {
        return "Move to nearest health pack";
    }
    QString getSyntax() const override { return "'heal'"; }

private:
    InputController& controller;
};

#endif // HEALCOMMAND_H
