#ifndef ATTACKCOMMAND_H
#define ATTACKCOMMAND_H

#include <climits>
#include "Command.h"
#include "InputController.h"

class AttackCommand : public Command {
public:
    AttackCommand(InputController& controller) : controller(controller) {}

    void execute(const QStringList& args) override {
        // Find nearest enemy and attack logic here
        auto level = GameModel::getInstance().getCurrentLevel();
        if (!level) return;

        auto protagonist = level->getProtagonist();
        if (!protagonist) return;

        // Find nearest enemy
        auto enemies = level->getEnemies();
        int minDist = INT_MAX;
        std::shared_ptr<EnemyModel> nearestEnemy = nullptr;

        for (const auto& enemy : enemies) {
            int dist = std::abs(enemy->getXPos() - protagonist->getXPos()) +
                       std::abs(enemy->getYPos() - protagonist->getYPos());
            if (dist < minDist) {
                minDist = dist;
                nearestEnemy = enemy;
            }
        }

        if (nearestEnemy) {
            controller.processGotoCommand(QStringList{
                "GOTO",
                QString::number(nearestEnemy->getXPos()),
                QString::number(nearestEnemy->getYPos())
            });
        }
    }

    QString getName() const override { return "attack"; }
    QString getDescription() const override {
        return "Attack nearest enemy";
    }
    QString getSyntax() const override { return "'attack'"; }

private:
    InputController& controller;
};

#endif // ATTACKCOMMAND_H
