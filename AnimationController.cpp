#include "AnimationController.h"
#include "QDebug"

AnimationController::AnimationController(QObject* parent)
    : QObject(parent)
{
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &AnimationController::onAnimationFrame);
    animationTimer->start(100);
}

void AnimationController::handleStatusChange(ProtagonistStatus newStatus) {
    qDebug() << "handleStatusChange called with status:" << static_cast<int>(newStatus);

    currentAnimation = getAnimationForStatus(newStatus);
    currentFrame = 0;
    animationTimer->setInterval(animations[currentAnimation].frameDelay);
}

void AnimationController::onAnimationFrame() {
    const auto& info = animations[currentAnimation];
    currentFrame++;

    if (currentFrame >= info.frameCount) {
        if (info.isLooping) {
            currentFrame = 0;
        } else {
            currentFrame = info.frameCount - 1;
            emit animationCompleted();
            return;
        }
    }
    emit frameChanged();
}

QRect AnimationController::getSpriteRect() {
    const auto& info = animations[currentAnimation];
    int rowIndex = currentFrame / info.framesPerRow;
    int row = info.rows[rowIndex];
    int column = currentFrame % info.framesPerRow;

    return QRect(
        column * info.spriteWidth,
        row * info.spriteHeight,
        info.spriteWidth,
        info.spriteHeight);
}

AnimationController::AnimationType AnimationController::getAnimationForStatus(ProtagonistStatus status) {
    switch (status) {
    case ProtagonistStatus::IDLE:
        return AnimationType::IDLE;
    case ProtagonistStatus::WALKING:
        return AnimationType::WALKING;
    case ProtagonistStatus::ATTACKING:
        qDebug() << "Protagonist dir:"<< model.getCurrentLevel()->getProtagonist()->getDirection();
        switch(model.getCurrentLevel()->getProtagonist()->getDirection()){
        case UP:
            return AnimationType::ATTACK_UP;
        case DOWN:
            return AnimationType::ATTACK_DOWN;
        case LEFT:
            return AnimationType::ATTACK_RIGHT;
        case RIGHT:
            return AnimationType::ATTACK_RIGHT;
        default:
            return AnimationType::ATTACK_RIGHT;
        }
    case ProtagonistStatus::HURT:
        return AnimationType::IDLE;
    case ProtagonistStatus::DEFEATED:
        return AnimationType::DEFEATED;
    default:
        return AnimationType::IDLE;
    }
}
