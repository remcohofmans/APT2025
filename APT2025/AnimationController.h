#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QRect>
#include "PROTAGONIST_STATUS.h"
#include "GameModel.h"

class AnimationController : public QObject {
    Q_OBJECT
public:

    enum class AnimationType {
        IDLE,
        WALKING,
        ATTACK_RIGHT,
        ATTACK_DOWN,
        ATTACK_UP,
        DEFEATED
    };

    struct AnimationInfo {
        QVector<int> rows;
        int frameCount;
        int frameDelay;
        int spriteWidth;
        int spriteHeight;
        int framesPerRow;
        bool isLooping = true;
    };

    explicit AnimationController(QObject* parent = nullptr);
    QRect getSpriteRect();

signals:
    void frameChanged();
    void animationCompleted();

public slots:
    void handleStatusChange(ProtagonistStatus newStatus);

private slots:
    void onAnimationFrame();

private:
    GameModel& model = GameModel::getInstance();

    QTimer* animationTimer;
    int currentFrame = 0;
    AnimationType currentAnimation = AnimationType::IDLE;

    QMap<AnimationType, AnimationInfo> animations = {
                                                     {AnimationType::IDLE,         {{0}, 6, 150, 192, 192, 6}},
                                                     {AnimationType::WALKING,      {{1}, 6, 100, 192, 192, 6}},
                                                     {AnimationType::ATTACK_RIGHT, {{2,3}, 12, 80, 192, 192, 6}},
                                                     {AnimationType::ATTACK_DOWN,  {{4,5}, 12, 80, 192, 192, 6}},
                                                     {AnimationType::ATTACK_UP,    {{6, 7}, 12, 80, 192, 192, 6}},
                                                     {AnimationType::DEFEATED,    {{0, 1}, 12, 80, 128, 128, 7, false}},
                                                     };
    AnimationType getAnimationForStatus(ProtagonistStatus status);
};

#endif // ANIMATIONCONTROLLER_H
