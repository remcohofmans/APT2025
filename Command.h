#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>

// Command interface
class Command {
public:
    virtual ~Command() = default;
    virtual void execute(const QStringList& args) = 0;
    virtual QString getName() const = 0;
    virtual QString getDescription() const = 0;
    virtual QString getSyntax() const = 0;
};
#endif // COMMAND_H
