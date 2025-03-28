QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++20

# Uncomment to disable APIs deprecated before Qt 6.0.0
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    AnimationController.cpp \
    ChasingEnemyModel.cpp \
    EnemyModel.cpp \
    EntityRenderer.cpp \
    GameOverDialog.cpp \
    GamePathFinder.cpp \
    GraphicalView.cpp \
    InputController.cpp \
    Level.cpp \
    LevelManager.cpp \
    MainWindow.cpp \
    OverlayView.cpp \
    PEnemyModel.cpp \
    ProtagonistModel.cpp \
    SpriteManager.cpp \
    TextBasedView.cpp \
    TileModel.cpp \
    ViewController.cpp \
    main.cpp \
    CollisionHandler.cpp \
    MovementManager.cpp \
    GameModel.cpp \

HEADERS += \
    AnimationController.h \
    AttackCommand.h \
    ChasingEnemyModel.h \
    Command.h \
    CommandRegistry.h \
    DIRECTION.h \
    ENEMY_STATUS.h \
    EnemyModel.h \
    EntityRenderer.h \
    GameOverDialog.h \
    GamePathFinder.h \
    GoToCommand.h \
    GraphicalView.h \
    HealCommand.h \
    HelpCommand.h \
    InputController.h \
    Level.h \
    LevelManager.h \
    MainWindow.h \
    MoveCommand.h \
    OverlayView.h \
    PEnemyModel.h \
    PROTAGONIST_STATUS.h \
    PathNode.h \
    ProtagonistModel.h \
    SpriteManager.h \
    TextBasedView.h \
    CollisionHandler.h \
    TileModel.h \
    View.h \
    ViewController.h \
    XEnemy.h \
    MovementManager.h \
    GameModel.h

FORMS += \
    MainWindow.ui

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Platform-specific library configurations
# Library path for linking
win32: CONFIG(release, debug|release): LIBS += -L$$PWD/../worldlib_source/libInstall/ -lworld
else:win32: CONFIG(debug, debug|release): LIBS += -L$$PWD/../worldlib_source/libInstall/ -lworld

else:unix: LIBS += -L$$PWD/../../../GroupT/Master/APT/Labs/Final_Project/worldlib/ -lworld

# Include paths for headers in libInstall/include
INCLUDEPATH += $$PWD/../worldlib_source/libInstall/include

DEPENDPATH += $$PWD/../../../GroupT/Master/APT/Labs/Final_Project/worldlib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../GroupT/Master/APT/Labs/Final_Project/worldlib/release/ -lworld
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../GroupT/Master/APT/Labs/Final_Project/worldlib/debug/ -lworld
else:unix: LIBS += -L$$PWD/../../../GroupT/Master/APT/Labs/Final_Project/worldlib/ -lworld

INCLUDEPATH += $$PWD/../../../GroupT/Master/APT/Labs/Final_Project/worldlib
DEPENDPATH += $$PWD/../../../GroupT/Master/APT/Labs/Final_Project/worldlib

RESOURCES += \
    testmap.qrc



