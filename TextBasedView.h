// TextBasedView.h
#ifndef TEXTBASEDVIEW_H
#define TEXTBASEDVIEW_H

#include "View.h"
#include "PROTAGONIST_STATUS.h"
#include <QTextEdit>
#include <QString>
#include <QVector>
#include <QTimer>

class TextBasedView : public View {
    Q_OBJECT

public:
    explicit TextBasedView(QWidget *parent = nullptr);
    void visualizeWorld() override;
    ~TextBasedView();

    void updatePathTiles(const std::vector<std::pair<int, int>>& newPath);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QString generateTextRepresentation();
    void adjustFontSize();
    void startAttackAnimation();
    void updateAttackAnimation();
    void startDeathAnimation(int x, int y);
    void updateDeathAnimation();
    void updateProtagonistAnimation();
    QString getColoredText(const QString& text, const QString& color);
    QString getColoredTextWithPoison(const QString& text, const QString& baseColor, bool isPoisoned, float poisonLevel);
    QString getAsciiForGrayValue(float value) const;

    std::vector<std::pair<int, int>> pathTiles;  // Store the current path

    static const int GRID_WIDTH = 30;
    static const int GRID_HEIGHT = 30;
    QTextEdit* textOutput;
    QVector<QVector<QString>> grid;
    QTimer* healingAnimTimer;
    bool isHealing = false;

    const QVector<QString> PROTAGONIST_FRAMES = {
        "-o-",  // Arms spread
        "\\o/", // Arms up with double backslash
    };
    struct DeathAnim {
        int x, y;
        int frame;
    };
    const QVector<QString> GRAYSCALE_CHARS = {" ~ ", " : ", " ^ ", " ▀ ", " m ", " * ", " % ", " = ", " # "};
    QVector<DeathAnim> deathAnimations;

    // Animation constants
    const QVector<QString> ATTACK_FRAMES = {" o ", "\\o/", "/o\\", "-o>", "&lt;o-" }; // { NEUTRAL(0), UP(1), DOWN (2), LEFT(3), RIGHT(4) }
    const QVector<QString> DEATH_FRAMES = {" ✨ ", " 💥 ", " 💫 ", " ⭐ ", " ✨ "};

    // Animation properties
    int protagonistAnimFrame = 0;
    QTimer* protagonistAnimTimer;
    QTimer* attackAnimTimer;
    QTimer* deathAnimTimer;
    QTimer* protagonistHitTimer;
    bool isProtagonistHit = false;
    int attackAnimFrame;

private slots:
    void onEnemyDeath();

public slots:
    void protagonistStatusUpdated(ProtagonistStatus status);
    void onHealthChanged(int damage);
    void onCurrentLevelChanged();
};

#endif // TEXTBASEDVIEW_H
