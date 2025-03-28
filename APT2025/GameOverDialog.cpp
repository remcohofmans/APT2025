#include "GameOverDialog.h"

GameOverDialog::GameOverDialog(QWidget *parent, GameModel& model)
    : QMessageBox(parent), model(model) {

    levelController = model.getLevelManager();
    setupStyle();
}

void GameOverDialog::setupStyle() {
    setStyleSheet(
        "QMessageBox {"
        "   background-color: #2C2C2C;"
        "   border: 2px solid #FF4C4C;"
        "   border-radius: 10px;"
        "}"
        "QMessageBox QLabel {"
        "   color: #FF4C4C;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "   min-width: 300px;"
        "   padding: 20px;"
        "}"
        "QPushButton {"
        "   background-color: #FF4C4C;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 10px 20px;"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #FF6B6B;"
        "}"
        );
}

void GameOverDialog::showGameEvent(const QString& reason) {
    setWindowTitle("GAME OVER");
    setIcon(QMessageBox::NoIcon);

    QString message = reason == "health" ?
                          "💀 YOUR HEALTH HAS REACHED ZERO!\nTime to start over, warrior!" :
                          "⚡ ENERGY DEPLETED!\nYour journey ends here... for now!";

    setText(message);
    setStandardButtons(QMessageBox::Reset);
    button(QMessageBox::Reset)->setText("RESTART GAME");

    connect(this, &QMessageBox::buttonClicked, this,
            [this](QAbstractButton* button) {
                if (standardButton(button) == QMessageBox::Reset) {
            //levelController->clearLevelStates();
                    levelController->loadLevel(1);
                }
            });

    exec();
}
