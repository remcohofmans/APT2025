#ifndef GOTOCOMMAND_H
#define GOTOCOMMAND_H

#include "Command.h"
#include "InputController.h"

class GotoCommand : public Command {
public:
    GotoCommand(InputController& controller) : controller(controller) {}

    void execute(const QStringList& args) override {
        if (args.size() != 2) return;
        controller.processGotoCommand(QStringList{"GOTO", args[0], args[1]});
    }

    QString getName() const override { return "goto"; }
    QString getDescription() const override {
        return "Move to specific coordinates";
    }
    QString getSyntax() const override { return "'goto x y'"; }

private:
    InputController& controller;
};

#endif // GOTOCOMMAND_H
