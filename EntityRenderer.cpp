#include "EntityRenderer.h"
#include <QTransform>
#include "PEnemyModel.h"
#include "ChasingEnemyModel.h"
#include "GameModel.h"
#include "QGraphicsColorizeEffect"

EntityRenderer::EntityRenderer(QGraphicsScene* scene, SpriteManager* spriteManager, QObject* parent)
    :QObject(parent),
    scene(scene),
    model(GameModel::getInstance()),
    spriteManager(spriteManager)
{
    connect(model.getCurrentLevel()->getProtagonist().get(), &ProtagonistModel::healthChanged, this, &EntityRenderer::handleHealthChange);


    protagonistHitTimer = new QTimer(this);
    healingAnimTimer = new QTimer(this);

    connect(protagonistHitTimer, &QTimer::timeout, this, [this]() {
        isProtagonistHit = false;
        if (protagonistItem) {
            protagonistItem->setGraphicsEffect(nullptr);
        }
    });

    connect(healingAnimTimer, &QTimer::timeout, this, [this]() {
        isHealing = false;
        if (protagonistItem) {
            protagonistItem->setGraphicsEffect(nullptr);
        }
    });
}

QPixmap EntityRenderer::flipPixmapHorizontally(const QPixmap& pixmap) {
    QTransform transform;
    transform.scale(-1, 1);
    return pixmap.transformed(transform);
}

void EntityRenderer::clearAllEntities() {
    // Clear protagonist
    if (protagonistItem) {
        scene->removeItem(protagonistItem.get());
        protagonistItem.reset();
    }

    // Clear enemies
    for (auto& item : enemyItems) {
        scene->removeItem(item.get());
    }
    for (auto& text : enemyHealthTexts) {
        scene->removeItem(text.get());
    }
    enemyItems.clear();
    enemyHealthTexts.clear();

    // Clear healt packs
    for (auto& item : healthPackItems) {
        scene->removeItem(item.get());
    }
    healthPackItems.clear();

    healthPackPositions.clear();
    enemyPositions.clear();
}


void EntityRenderer::renderProtagonist(int x, int y, const QRect& spriteRect, Direction direction, bool useDeathSpriteSheet) {
    if (!protagonistItem) {
        protagonistItem = std::make_shared<QGraphicsPixmapItem>();
        scene->addItem(protagonistItem.get());
    }

    // Constants for tile and sprite sizes
    const float TILE_SIZE = 10.0f;
    const float REGULAR_SPRITE_SIZE = 192.0f;
    const float DEATH_SPRITE_SIZE = 128.0f;

    QPixmap currentSprite;
    float spriteSize;
    float scale;

    if (useDeathSpriteSheet) {
        spriteSize = DEATH_SPRITE_SIZE;
        scale = TILE_SIZE / DEATH_SPRITE_SIZE;

        if (direction == Direction::LEFT) {
            currentSprite = flipPixmapHorizontally(spriteManager->getDeathSpritesheet().copy(spriteRect));
        } else {
            currentSprite = spriteManager->getDeathSpritesheet().copy(spriteRect);
        }
    } else {
        spriteSize = REGULAR_SPRITE_SIZE;
        scale = TILE_SIZE / REGULAR_SPRITE_SIZE;

        if (direction == Direction::LEFT) {
            currentSprite = flipPixmapHorizontally(spriteManager->getProtagonistSpritesheet().copy(spriteRect));
        } else {
            currentSprite = spriteManager->getProtagonistSpritesheet().copy(spriteRect);
        }
    }

    float scaledSize = spriteSize * scale * 2.0f;
    float offset = (scaledSize - TILE_SIZE) / 2.0f;

    protagonistItem->setPixmap(currentSprite);
    protagonistItem->setPos(x * TILE_SIZE - offset, y * TILE_SIZE - offset);
    protagonistItem->setScale(scale * 2.0f);
    protagonistItem->setZValue(1001);

    if (isProtagonistHit) {
        auto effect = new QGraphicsColorizeEffect();
        effect->setColor(Qt::red);
        effect->setStrength(0.5);
        protagonistItem->setGraphicsEffect(effect);
    }
    else if (isHealing) {
        auto effect = new QGraphicsColorizeEffect();
        effect->setColor(Qt::green);
        effect->setStrength(0.5);
        protagonistItem->setGraphicsEffect(effect);
    }
    else {
        protagonistItem->setGraphicsEffect(nullptr);
    }
}

void EntityRenderer::showDamageEffect() {
    isProtagonistHit = true;
    if (protagonistItem) {
        auto effect = new QGraphicsColorizeEffect();
        effect->setColor(Qt::red);
        effect->setStrength(0.5);
        protagonistItem->setGraphicsEffect(effect);
    }
    protagonistHitTimer->start(1000);  // 1 second duration
}

void EntityRenderer::showHealingEffect() {
    isHealing = true;
    if (protagonistItem) {
        auto effect = new QGraphicsColorizeEffect();
        effect->setColor(Qt::green);
        effect->setStrength(0.5);
        protagonistItem->setGraphicsEffect(effect);
    }
    healingAnimTimer->start(1000);  // 1 second duration
}

void EntityRenderer::handleHealthChange(int damage) {
    if (damage > 0) {
        showDamageEffect();  // Show damage effect
    } else if (damage < 0) {
        showHealingEffect(); // Show healing effect
    }
}

void EntityRenderer::renderEnemies(const QVector<std::shared_ptr<EnemyModel>>& enemies) {
    enemyPositions.clear();
    QVector<std::shared_ptr<EnemyModel>> enemiesNeedingRelocation;

    // Identify invalid positions and enemies that need relocation
    for (const auto& enemy : enemies) {
        auto tile = model.getCurrentLevel()->getTileAt(enemy->getXPos(), enemy->getYPos());
        int tileValue = static_cast<int>(tile.getValue() * 255.0f);
        bool isValidTile = !(tileValue == 30 || tileValue == 0 || tileValue == 128 || tileValue == 255);

        // Relocate if current position is invalid
        if (!isValidTile) {
            enemiesNeedingRelocation.push_back(enemy);
        } else {
            std::pair<int, int> pos = {enemy->getXPos(), enemy->getYPos()};
            enemyPositions.insert(pos);
        }
    }

    // Relocate enemies to valid positions
    for (auto& enemy : enemiesNeedingRelocation) {
        bool found = false;
        // Search in expanding radius from original position
        for (int radius = 1; radius < 20 && !found; radius++) {
            for (int dy = -radius; dy <= radius && !found; dy++) {
                for (int dx = -radius; dx <= radius && !found; dx++) {
                    int newX = enemy->getXPos() + dx;
                    int newY = enemy->getYPos() + dy;

                    // Check if position is within bounds
                    if (newX < 0 || newY < 0 ||
                        newX >= model.getCurrentLevel()->getMapCols() ||
                        newY >= model.getCurrentLevel()->getMapRows()) {
                        continue;
                    }

                    auto tile = model.getCurrentLevel()->getTileAt(newX, newY);
                    int tileValue = static_cast<int>(tile.getValue() * 255);

                    // Check if new position is valid and unoccupied
                    bool isValidTile = !(tileValue == 30 || tileValue == 0 || tileValue == 128 || tileValue == 255);
                    std::pair<int, int> newPos = {newX, newY};

                    if (isValidTile && !healthPackPositions.contains(newPos) && !enemyPositions.contains(newPos)) {
                        // Update enemy position
                        enemy->setXPos(newX);
                        enemy->setYPos(newY);
                        enemyPositions.insert(newPos);
                        found = true;
                    }
                }
            }
        }
    }

    // Rest of the original rendering code
    for (const auto& enemy : enemies) {
        std::pair<int, int> pos = {enemy->getXPos(), enemy->getYPos()};

        if (!enemyItems.contains(pos)) {
            auto enemyItem = std::make_shared<QGraphicsPixmapItem>();
            enemyItems[pos] = enemyItem;
            scene->addItem(enemyItem.get());

            auto healthText = std::make_shared<QGraphicsTextItem>();
            enemyHealthTexts[pos] = healthText;
            scene->addItem(healthText.get());
        }

        auto& enemyItem = enemyItems[pos];
        auto& healthText = enemyHealthTexts[pos];

        // Update enemy appearance
        if (enemy->getDefeated()) {
            enemyItem->setPixmap(spriteManager->getDefeatedSprite());
            enemyItem->setScale(10.0 / spriteManager->getDefeatedSprite().width());
            enemyItem->setPos(enemy->getXPos() * 10, enemy->getYPos() * 10);
        } else if (dynamic_cast<PEnemyModel*>(enemy.get())) {
            float scaleFactor = 10.0f / spriteManager->getHealthPackSprite().width() * 2.0f;
            float offset = (spriteManager->getHealthPackSprite().width() * scaleFactor - 10.0f) / 2.0f;

            enemyItem->setPixmap(spriteManager->getPoisonEnemySprite());
            enemyItem->setPos((enemy->getXPos() * 10) - offset, (enemy->getYPos() * 10) - offset);
            enemyItem->setScale(10.0 / spriteManager->getPoisonEnemySprite().height()*2);

        } else if (dynamic_cast<ChasingEnemyModel*>(enemy.get())) {
            enemyItem->setPixmap(spriteManager->getXEnemySprite());
            enemyItem->setScale(10.0 / spriteManager->getXEnemySprite().width());
            enemyItem->setPos(enemy->getXPos() * 10, enemy->getYPos() * 10);

        } else {
            enemyItem->setPixmap(spriteManager->getEnemySprite());
            enemyItem->setScale(10.0 / spriteManager->getEnemySprite().width());
            enemyItem->setPos(enemy->getXPos() * 10, enemy->getYPos() * 10);
        }

        enemyItem->setZValue(1001);

        healthText->setPlainText(QString::number(enemy->getValue()));
        healthText->setDefaultTextColor(Qt::red);
        healthText->setFont(QFont("Comic Sans", 5));
        healthText->setPos(enemy->getXPos() * 10, (enemy->getYPos() * 10) - 10);
        healthText->setZValue(1002);
    }

    // Clean up removed enemies
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsPixmapItem>>::iterator it;
    for (it = enemyItems.begin(); it != enemyItems.end();) {
        if (!enemyPositions.contains(it.key())) {
            scene->removeItem(it.value().get());
            scene->removeItem(enemyHealthTexts[it.key()].get());
            enemyHealthTexts.remove(it.key());
            it = enemyItems.erase(it);
        } else {
            ++it;
        }
    }
}

void EntityRenderer::renderHealthPacks(const QVector<std::shared_ptr<TileModel>>& healthPacks) {
    healthPackPositions.clear();
    QVector<std::shared_ptr<TileModel>> healthPacksNeedingRelocation;

    // Identify invalid positions and enemies that need relocation
    for (const auto& healthPack : healthPacks) {
        auto tile = model.getCurrentLevel()->getTileAt(healthPack->getXPos(), healthPack->getYPos());
        int tileValue =static_cast<int>(tile.getValue() * 255);
        bool isValidTile = !(tileValue == 30 || tileValue == 0 || tileValue == 128 || tileValue == 255);

        // Relocate if current position is invalid
        if (!isValidTile) {
            healthPacksNeedingRelocation.push_back(healthPack);
        } else {
            std::pair<int, int> pos = {healthPack->getXPos(), healthPack->getYPos()};
            healthPackPositions.insert(pos);
        }
    }

    // Relocate health packs to valid positions
    for (auto& healthPack : healthPacksNeedingRelocation) {
        bool found = false;
        // Search in expanding radius from original position
        for (int radius = 1; radius < 20 && !found; radius++) {
            for (int dy = -radius; dy <= radius && !found; dy++) {
                for (int dx = -radius; dx <= radius && !found; dx++) {
                    int newX = healthPack->getXPos() + dx;
                    int newY = healthPack->getYPos() + dy;

                    // Check if position is within bounds
                    if (newX < 0 || newY < 0 ||
                        newX >= model.getCurrentLevel()->getMapCols() ||
                        newY >= model.getCurrentLevel()->getMapRows()) {
                        continue;
                    }

                    auto tile = model.getCurrentLevel()->getTileAt(newX, newY);
                    int tileValue = static_cast<int>(tile.getValue() * 255);

                    // Check if new position is valid and unoccupied
                    bool isValidTile = !(tileValue == 30 || tileValue == 0 || tileValue == 128 || tileValue == 255);
                    std::pair<int, int> newPos = {newX, newY};

                    if (isValidTile && !healthPackPositions.contains(newPos) && !enemyPositions.contains(newPos)) {
                        // Update enemy position
                        healthPack->setXPos(newX);
                        healthPack->setYPos(newY);
                        healthPackPositions.insert(newPos);
                        found = true;
                    }
                }
            }
        }
    }

    // Rest of the original rendering code
    for (const auto& healthPack : healthPacks) {
        std::pair<int, int> pos = {healthPack->getXPos(), healthPack->getYPos()};
        if (!healthPackItems.contains(pos)) {
            auto healthPackItem = std::make_shared<QGraphicsPixmapItem>();
            healthPackItems[pos] = healthPackItem;
            scene->addItem(healthPackItem.get());
        }

        float healthScaleFactor = 10.0f / spriteManager->getHealthPackSprite().width() * 2.0f;
        float healthOffset = (spriteManager->getHealthPackSprite().width() * healthScaleFactor - 10.0f) / 2.0f;

        auto& healthPackItem = healthPackItems[pos];
        healthPackItem->setPixmap(spriteManager->getHealthPackSprite());
        healthPackItem->setPos((healthPack->getXPos() * 10) - healthOffset, (healthPack->getYPos() * 10) - healthOffset);
        healthPackItem->setScale(10.0 / 64);
        healthPackItem->setZValue(1001);
    }

    // Remove items for health packs that no longer exist
    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsPixmapItem>>::iterator it;
    for (it = healthPackItems.begin(); it != healthPackItems.end();) {
        if (!healthPackPositions.contains(it.key())) {
            scene->removeItem(it.value().get());
            it = healthPackItems.erase(it);
        } else {
            ++it;
        }
    }
}

void EntityRenderer::reRenderEnemies(const QVector<std::shared_ptr<EnemyModel>>& enemies) {
    QSet<std::pair<int, int>> currentPositions;

    // Update or create enemy items
    for (const auto& enemy : enemies) {
        auto tile = model.getCurrentLevel()->getTileAt(enemy->getXPos(), enemy->getYPos());

        int tileValue = static_cast<int>(tile.getValue() * 255);        // Skip if tile value is 30 (wall), 0 (sea), or 128 (inaccessible)
        if (tileValue == 30 || tileValue == 0 || tileValue == 128 || tileValue == 255) {
            continue;
        }

        std::pair<int, int> pos = {enemy->getXPos(), enemy->getYPos()};
        currentPositions.insert(pos);

        if (!enemyItems.contains(pos)) {
            auto enemyItem = std::make_shared<QGraphicsPixmapItem>();
            enemyItems[pos] = enemyItem;
            scene->addItem(enemyItem.get());

            auto healthText = std::make_shared<QGraphicsTextItem>();
            enemyHealthTexts[pos] = healthText;
            scene->addItem(healthText.get());
        }

        auto& enemyItem = enemyItems[pos];
        auto& healthText = enemyHealthTexts[pos];

        // Update enemy appearance
        if (enemy->getDefeated()) {
            enemyItem->setPixmap(spriteManager->getDefeatedSprite());
            enemyItem->setScale(10.0 / spriteManager->getDefeatedSprite().width());
            enemyItem->setPos(enemy->getXPos() * 10, enemy->getYPos() * 10);

        } else if (dynamic_cast<PEnemyModel*>(enemy.get())) {

            float scaleFactor = 10.0f / spriteManager->getHealthPackSprite().width() * 2.0f;
            float offset = (spriteManager->getHealthPackSprite().width() * scaleFactor - 10.0f) / 2.0f;

            enemyItem->setPixmap(spriteManager->getPoisonEnemySprite());
            enemyItem->setPos((enemy->getXPos() * 10) - offset, (enemy->getYPos() * 10) - offset);
            enemyItem->setScale(10.0 / spriteManager->getPoisonEnemySprite().height()*2);

        } else if (dynamic_cast<ChasingEnemyModel*>(enemy.get())) {
            enemyItem->setPixmap(spriteManager->getXEnemySprite());
            enemyItem->setScale(10.0 / spriteManager->getXEnemySprite().width());
            enemyItem->setPos(enemy->getXPos() * 10, enemy->getYPos() * 10);

        } else {
            enemyItem->setPixmap(spriteManager->getEnemySprite());
            enemyItem->setScale(10.0 / spriteManager->getEnemySprite().width());
            enemyItem->setPos(enemy->getXPos() * 10, enemy->getYPos() * 10);

        }

        enemyItem->setZValue(1001);

        healthText->setPlainText(QString::number(enemy->getValue()));
        healthText->setDefaultTextColor(Qt::red);
        healthText->setFont(QFont("Comic Sans", 5));
        healthText->setPos(enemy->getXPos() * 10, (enemy->getYPos() * 10) - 10);
        healthText->setZValue(1002);
    }

    QMap<std::pair<int, int>, std::shared_ptr<QGraphicsPixmapItem>>::iterator it;
    for (it = enemyItems.begin(); it != enemyItems.end();) {
        if (!currentPositions.contains(it.key())) {
            scene->removeItem(it.value().get());
            scene->removeItem(enemyHealthTexts[it.key()].get());
            enemyHealthTexts.remove(it.key());
            it = enemyItems.erase(it);
        } else {
            ++it;
        }
    }
}
