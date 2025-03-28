// In gameoverdialog.h
#ifndef GAMEOVERDIALOG_H
#define GAMEOVERDIALOG_H

#include <QMessageBox>
#include <QAbstractButton>
#include "LevelManager.h"
#include "GameModel.h"

class GameOverDialog : public QMessageBox {
    Q_OBJECT

public:
    GameOverDialog(QWidget *parent, GameModel& model);
    void showGameEvent(const QString& reason);

private:
    std::shared_ptr<LevelManager> levelController;
    GameModel& model;
    void setupStyle();
};

#endif
