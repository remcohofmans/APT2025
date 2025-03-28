#ifndef COMMANDREGISTRY_H
#define COMMANDREGISTRY_H

#include <QMap>
#include <memory>
#include <QDebug>
#include <QMessageBox>
#include "Command.h"

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }

    void registerCommand(std::unique_ptr<Command> command) {
        QString name = command->getName().toLower();
        commands[name] = std::move(command);
    }

    bool executeCommand(const QString& input) {
        QStringList parts = input.trimmed().split(" ", Qt::SkipEmptyParts);
        if (parts.isEmpty()) return false;

        QString commandName = parts[0].toLower();
        parts.removeFirst(); // Remove command name, leaving only arguments

        // Find command by prefix matching
        QStringList matchedCommands;
        for (const auto& name : commands.keys()) {
            if (name.startsWith(commandName)) {
                matchedCommands.append(name);
            }
        }

        if (matchedCommands.size() == 1) {
            // Exactly one match, execute it
            commands[matchedCommands[0]]->execute(parts);
            return true;
        } else if (matchedCommands.size() > 1) {
            // Multiple matches, suggest them
            // Here, you could print suggestions or handle ambiguity.
            qDebug() << "Ambiguous command. Possible matches:";
            for (const QString& match : matchedCommands) {
                qDebug() << match;
            }
            return false;
        }

        return false; // No match found
    }

    QStringList getCompletions(const QString& partial) const {
        QStringList completions;
        QString prefix = partial.toLower();
        for (const auto& name : commands.keys()) {
            if (name.startsWith(prefix)) {
                completions << name;
            }
        }
        return completions;
    }

    QString getHelpText(QWidget* parent = nullptr) const {
        QString help = "<h2>Available Commands</h2>\n<ul>";

        for (const auto& command : commands) {
            help += QString(
                        "<li><b>%1</b><br>\n"
                        "<i>Description:</i> %2<br>\n"
                        "<i>Syntax:</i> <code>%3</code></li>\n"
                        )
                        .arg(command->getName().toHtmlEscaped())
                        .arg(command->getDescription().toHtmlEscaped())
                        .arg(command->getSyntax().toHtmlEscaped());
        }

        help += "</ul>";

        // Show help text in a message box with rich text support
        QMessageBox messageBox(parent);
        messageBox.setWindowTitle("Help");
        messageBox.setTextFormat(Qt::RichText);
        messageBox.setText(help);
        messageBox.setIcon(QMessageBox::Information);
        messageBox.exec();

        return help; // Could be used in the future
    }



private:
    QMap<QString, std::shared_ptr<Command>> commands;
    CommandRegistry() = default;
};

#endif // COMMANDREGISTRY_H
