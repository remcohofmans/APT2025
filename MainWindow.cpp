#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QAbstractButton>
#include <qpushbutton.h>
#include "MainWindow.h"
#include "ViewController.h"
#include "ui_MainWindow.h"
#include "GameOverDialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    model(GameModel::getInstance()),
    ui(new Ui::MainWindow)
{
    GameModel& model = GameModel::getInstance();
    model.initializeLevelManager();

    model.getLevelManager()->loadLevel(3);

    ui->setupUi(this);

    if (!GameModel::getInstance().getCurrentLevel()) {
        qFatal("GameModel initialization failed: currentLevel is null.");
    }

    // Main layout setup
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // Left panel for status and controls (except input controller)
    QWidget *leftPanel = new QWidget(this);
    QVBoxLayout *leftPanelLayout = new QVBoxLayout(leftPanel);
    leftPanelLayout->setSpacing(12);
    leftPanelLayout->setContentsMargins(10, 10, 10, 10);

    // Status group box (Health and Energy meters)
    QGroupBox *statusGroup = new QGroupBox("Character Status", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setSpacing(8);

    // Health meter layout
    QWidget *healthWidget = new QWidget(this);
    QHBoxLayout *healthLayout = new QHBoxLayout(healthWidget);
    healthLayout->setSpacing(6);
    QLabel *healthLabel = new QLabel("Health:", this);
    healthLabel->setMinimumWidth(60);
    healthMeter = new QProgressBar(this);
    healthMeter->setRange(0, 100);
    healthMeter->setValue(model.getCurrentLevel()->getProtagonist()->getHealth());
    healthMeter->setTextVisible(true);
    healthMeter->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid grey;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    background-color: #ffffff;"
        "    height: 20px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #ff0000;"
        "    border-radius: 3px;"
        "}"
        );
    healthLayout->addWidget(healthLabel);
    healthLayout->addWidget(healthMeter);

    // Energy meter layout
    QWidget *energyWidget = new QWidget(this);
    QHBoxLayout *energyLayout = new QHBoxLayout(energyWidget);
    energyLayout->setSpacing(6);
    QLabel *energyLabel = new QLabel("Energy:", this);
    energyLabel->setMinimumWidth(60);
    energyMeter = new QProgressBar(this);
    energyMeter->setRange(0, 100);
    energyMeter->setValue(model.getCurrentLevel()->getProtagonist()->getEnergy());
    energyMeter->setTextVisible(true);
    energyMeter->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid grey;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    background-color: #ffffff;"
        "    height: 20px;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #0000ff;"
        "    border-radius: 3px;"
        "}"
        );
    energyLayout->addWidget(energyLabel);
    energyLayout->addWidget(energyMeter);

    statusLayout->addWidget(healthWidget);
    statusLayout->addWidget(energyWidget);
    leftPanelLayout->addWidget(statusGroup);

    // Controls group box (without input controller)
    QGroupBox *controlsGroup = new QGroupBox("Controls", this);
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsGroup);
    controlsLayout->setSpacing(8);

    // Text input
    textInput = new QLineEdit(this);
    textInput->setPlaceholderText("Enter command...");
    textInput->setStyleSheet(
        "QLineEdit {"
        "    padding: 6px;"
        "    border: 2px solid grey;"
        "    border-radius: 5px;"
        "    background-color: #ffffff;"
        "}"
        );
    controlsLayout->addWidget(textInput);

    // Button container
    QWidget *buttonsWidget = new QWidget(this);
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonsWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);

    // Send button
    sendButton = new QPushButton("Send", this);
    sendButton->setStyleSheet(
        "QPushButton {"
        "    padding: 6px 12px;"
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        );
    buttonLayout->addWidget(sendButton);

    // Toggle view button
    toggleViewButton = new QPushButton("Switch to Text-Based View", this);
    toggleViewButton->setStyleSheet(
        "QPushButton {"
        "    padding: 6px 12px;"
        "    background-color: #2196F3;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #1976D2;"
        "}"
        );
    buttonLayout->addWidget(toggleViewButton);

    controlsLayout->addWidget(buttonsWidget);
    leftPanelLayout->addWidget(controlsGroup);

    // Toggle overlay button
    QPushButton *toggleOverlayButton = new QPushButton("Toggle Overlay", this);
    toggleOverlayButton->setStyleSheet(
        "QPushButton {"
        "    padding: 6px 12px;"
        "    background-color: #FF9800;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #F57C00;"
        "}"
        );
    QHBoxLayout *overlayButtonLayout = new QHBoxLayout();
    overlayButtonLayout->addWidget(toggleOverlayButton);
    overlayButtonLayout->addStretch();
    leftPanelLayout->addLayout(overlayButtonLayout);

    // Right panel - View and Input Controller
    QWidget *rightPanel = new QWidget(this);
    QVBoxLayout *rightPanelLayout = new QVBoxLayout(rightPanel);
    rightPanelLayout->setSpacing(12);
    rightPanelLayout->setContentsMargins(10, 10, 10, 10);

    // View setup
    viewStack = new QStackedWidget(this);

    // Graphical view
    graphicalView = new GraphicalView(this);
    graphicalView->visualizeWorld();
    viewStack->addWidget(graphicalView);

    // Text-based view
    textBasedView = new TextBasedView(this);
    textBasedView->visualizeWorld();
    viewStack->addWidget(textBasedView);

    graphicalView->makeOverlay(model.getCurrentLevel()->getOverlay());

    rightPanelLayout->addWidget(viewStack);

    // Input controller
    inputController = new InputController(this);
    rightPanelLayout->addWidget(inputController);

    viewController = new ViewController(this);
    viewController->setView(graphicalView);

    // Add panels to splitter
    splitter->addWidget(leftPanel);
    splitter->addWidget(rightPanel);

    // Set initial split sizes
    QList<int> sizes;
    sizes << 250 << 550;
    splitter->setSizes(sizes);

    setCentralWidget(splitter);

    // Connect signals
    connect(&*model.getCurrentLevel()->getProtagonist(), &ProtagonistModel::posChanged, viewController, &ViewController::ProtagonistMoved);
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::sendButtonTriggered);
    connect(toggleViewButton, &QPushButton::clicked, this, &MainWindow::toggleView);
    connect(model.getLevelManager().get(), &LevelManager::levelLoaded, this, &MainWindow::handleLevelChange);
    connect(model.getCurrentLevel()->getProtagonist().get(), &ProtagonistModel::healthChanged, this, &MainWindow::updateHealthMeter);
    connect(model.getCurrentLevel()->getProtagonist().get(), &ProtagonistModel::energyChanged, this, &MainWindow::updateEnergyMeter);
    connect(toggleOverlayButton, &QPushButton::clicked, this, &MainWindow::toggleOverlay);
    connect(graphicalView, &GraphicalView::tileClicked, inputController, &InputController::handleTileClick);
    connect(model.getLevelManager().get(), &LevelManager::levelLoaded, graphicalView, &GraphicalView::refreshScene);
    connect(&*model.getCurrentLevel()->getProtagonist(), &ProtagonistModel::statusChanged, textBasedView, &TextBasedView::protagonistStatusUpdated);
    connect(model.getCurrentLevel().get(), &Level::poisonStatusChanged, graphicalView, &GraphicalView::updateTileColors);
    connect(model.getCurrentLevel().get(), &Level::poisonStatusChanged, textBasedView, &TextBasedView::visualizeWorld);
    connect(model.getCurrentLevel().get(), &Level::enemyMoved, graphicalView, &GraphicalView::refreshEnemies); //maak hier betere func voor
    connect(inputController, &InputController::pathUpdated,
            graphicalView, &GraphicalView::setPathTiles);
    connect(inputController, &InputController::pathUpdated,
            textBasedView, &TextBasedView::updatePathTiles);

    setMinimumSize(800, 600);
    inputController->setFocus();
    textInput->installEventFilter(this);
}

void MainWindow::handleLevelChange(std::shared_ptr<Level> level) {
    qDebug() << "Current level valid:" << (model.getCurrentLevel() != nullptr);
    if (model.getCurrentLevel()) {
        qDebug() << "Current level protagonist valid:" << (model.getCurrentLevel()->getProtagonist() != nullptr);
    }
    qDebug() << "New level valid:" << (level != nullptr);
    if (level) {
        qDebug() << "New level protagonist valid:" << (level->getProtagonist() != nullptr);
    }    // Disconnect old connections if they exist
    if (auto oldProtagonist = model.getCurrentLevel()->getProtagonist().get()) {
        disconnect(oldProtagonist, &ProtagonistModel::healthChanged,
                   this, &MainWindow::updateHealthMeter);
        disconnect(oldProtagonist, &ProtagonistModel::energyChanged,
                   this, &MainWindow::updateEnergyMeter);
        disconnect(oldProtagonist, &ProtagonistModel::statusChanged,
                   textBasedView, &TextBasedView::protagonistStatusUpdated);
    }

    // Connect signals for the new level's protagonist
    if (auto newProtagonist = level->getProtagonist().get()) {
        connect(newProtagonist, &ProtagonistModel::healthChanged,
                this, &MainWindow::updateHealthMeter);
        connect(newProtagonist, &ProtagonistModel::energyChanged,
                this, &MainWindow::updateEnergyMeter);
        connect(newProtagonist, &ProtagonistModel::statusChanged,
                textBasedView, &TextBasedView::protagonistStatusUpdated);
    }

    connect(model.getCurrentLevel().get(), &Level::poisonStatusChanged,
            graphicalView, &GraphicalView::updateTileColors);
}

void MainWindow::checkGameOver(int value, const QString& type) {
    if (value <= 0 && !isGameOver) {
        isGameOver = true;
        GameOverDialog dialog(this, model);
        dialog.showGameEvent(type);
        isGameOver = false;
    }
}

void MainWindow::updateHealthMeter(int damage) {
    int newValue = healthMeter->value() - damage;
    healthMeter->setValue(newValue);
    checkGameOver(newValue, "health");
}

void MainWindow::updateEnergyMeter(int newValue) {
    energyMeter->setValue(newValue);
    checkGameOver(newValue, "energy");
}

void MainWindow::sendButtonTriggered()
{
    QString inputText = textInput->text();
    if (!inputText.isEmpty()) {
        inputController->handleCommand(inputText);
        textInput->clear();
    }
}

void MainWindow::toggleView()
{
    int currentIndex = viewStack->currentIndex();
    viewStack->setCurrentIndex(1 - currentIndex);

    // Update button text
    toggleViewButton->setText(currentIndex == 0 ?
                                  "Switch to Graphical View" :
                                  "Switch to Text-Based View");

    // Set the correct view in ViewController
    if (currentIndex == 0) {
        viewController->setView(textBasedView);
    } else {
        viewController->setView(graphicalView);
    }
    inputController->setFocus(); // Ensure focus stays on input controller

}

void MainWindow::toggleOverlay(){
    graphicalView->toggleOverlay();
}

MainWindow::~MainWindow()
{
    delete ui;
}


