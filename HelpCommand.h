#ifndef HELPCOMMAND_H
#define HELPCOMMAND_H

#include "Command.h"
#include "CommandRegistry.h"
#include <QDebug>

class HelpCommand : public Command {
public:
    void execute(const QStringList& args) override {
        qDebug() << CommandRegistry::getInstance().getHelpText();
    }

    QString getName() const override { return "help"; }
    QString getDescription() const override {
        return "Show list of available commands";
    }
    QString getSyntax() const override { return "'help'"; }
};

#endif // HELPCOMMAND_H
