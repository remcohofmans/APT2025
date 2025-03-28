#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QSplitter>
#include "GraphicalView.h"
#include "TextBasedView.h"
#include "InputController.h"
#include "ViewController.h"
#include "LevelManager.h"

class GameModel;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    // Methods to update the health and energy meters
    void updateHealthMeter(int damage);
    void updateEnergyMeter(int newEnergy);
    void toggleOverlay();

private slots:
    void sendButtonTriggered();
    void toggleView();

private slots:
    void handleLevelChange(std::shared_ptr<Level> level);

private:
    // Initialize the GUI components
    void initializeWidgets();
    void setupLayouts();
    void setupConnections();

private:
    void checkGameOver(int value, const QString& type);
    void showGameOverDialog(const QString& reason);

    bool isGameOver;
    GameModel &model;

    Ui::MainWindow *ui;
    QStackedWidget *viewStack; // Allows switching between views
    QLineEdit *textInput;
    QPushButton *sendButton;
    QPushButton *toggleViewButton; // Button for switching views
    InputController *inputController;
    GraphicalView *graphicalView;
    TextBasedView *textBasedView;
    ViewController *viewController;
    LevelManager* levelManager;
    QProgressBar *healthMeter;
    QProgressBar *energyMeter;
};

#endif // MAINWINDOW_H
