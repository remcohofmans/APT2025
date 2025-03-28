// TextBasedView.cpp
#include "TextBasedView.h"
#include "LevelManager.h"
#include "PEnemyModel.h"
#include "ChasingEnemyModel.h"
#include <QVBoxLayout>
#include <QDebug>
#include <QFontMetrics>
#include <QResizeEvent>
#include <set>


TextBasedView::TextBasedView(QWidget *parent)
    : View(parent), textOutput(new QTextEdit(this)), attackAnimFrame(0)
{
    // Initialize timers
    protagonistAnimTimer = new QTimer(this);
    protagonistAnimTimer->start(250);  // Update every 250ms

    attackAnimTimer = new QTimer(this);

    deathAnimTimer = new QTimer(this);

    protagonistHitTimer = new QTimer(this);

    healingAnimTimer = new QTimer(this);


    // Connect signals and slots
    connect(model.getLevelManager()->getCurrentLevel()->getProtagonist().get(), &ProtagonistModel::healthChanged, this, &TextBasedView::onHealthChanged);
    connect(model.getLevelManager().get(), &LevelManager::levelLoaded, this, &TextBasedView::onCurrentLevelChanged);

    // Connect timer signals
    connect(protagonistAnimTimer, &QTimer::timeout, this, &TextBasedView::updateProtagonistAnimation);
    connect(attackAnimTimer, &QTimer::timeout, this, &TextBasedView::updateAttackAnimation);
    connect(deathAnimTimer, &QTimer::timeout, this, &TextBasedView::updateDeathAnimation);

    connect(protagonistHitTimer, &QTimer::timeout, this, [this]() {
        isProtagonistHit = false;  // Reset hit state
        protagonistHitTimer->stop(); // Stop the timer
        visualizeWorld(); // Redraw the world
    });

    connect(healingAnimTimer, &QTimer::timeout, this, [this]() {
        isHealing = false;
        healingAnimTimer->stop();
        visualizeWorld();
    });

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Configure text output
    textOutput->setReadOnly(true);
    textOutput->setWordWrapMode(QTextOption::NoWrap);
    textOutput->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);     // The text widget should only display content
    textOutput->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // within its fixed dimensions, no scrollbar is added!

    // Initialize with default font
    QFont monoFont("Monaco", 10);
    monoFont.setStyleHint(QFont::Monospace);    // Each character occupying the same amount of horizontal space
    textOutput->setFont(monoFont);

    // Configure size policy
    textOutput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Style the text output
    textOutput->setStyleSheet(
        "QTextEdit {"
        "    background-color: black;"
        "    color: white;"
        "    border: none;"
        "}"
        );

    // Setup layout
    layout->addWidget(textOutput);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    // Initialize grid with fixed size
    grid = QVector<QVector<QString>>(GRID_HEIGHT, QVector<QString>(GRID_WIDTH, " . "));

    adjustFontSize();
}

void TextBasedView::resizeEvent(QResizeEvent* event)
{
    View::resizeEvent(event);
    adjustFontSize();
    visualizeWorld();
}

void TextBasedView::adjustFontSize()
{
    // Get the widget dimensions
    int availableWidth = width();
    int availableHeight = height();

    QFont currentFont = textOutput->font();

    // Calculate the maximum possible font size that fits both dimensions
    double widthBasedSize = (availableWidth * 1.0) / (GRID_WIDTH * 3);  // 3 chars per cell
    double heightBasedSize = (availableHeight * 1.0) / GRID_HEIGHT;

    // Use the smaller of the two sizes to ensure everything fits
    int fontSize = qMin(widthBasedSize, heightBasedSize);

    // Set the calculated font size
    currentFont.setPointSize(std::max(1, fontSize+1));

    QFontMetrics fm(currentFont);
    //int cellWidth = fm.horizontalAdvance(' ') * 3;
    //int cellHeight = fm.height();

    // Get the line spacing, which includes leading (the space between lines)
    //int lineSpacing = fm.lineSpacing();

    // Output the line spacing (distance between lines)
    //qDebug() << "Line spacing (distance between lines):" << lineSpacing;

    // qDebug() << "Adjusted font size:" << fontSize
    //          << "\nCell dimensions - Width:" << cellWidth << "Height:" << cellHeight
    //          << "\nTotal dimensions - Width:" << (cellWidth * GRID_WIDTH)
    //          << "Height:" << (cellHeight * GRID_HEIGHT)
    //          << "\nAvailable space - Width:" << availableWidth
    //          << "Height:" << availableHeight;

    textOutput->setFont(currentFont);
}

QString TextBasedView::getColoredText(const QString& text, const QString& color) {
    return QString("<span style='color: %1;'>%2</span>").arg(color, text);
}

QString TextBasedView::getColoredTextWithPoison(const QString& text, const QString& baseColor, bool isPoisoned, float poisonLevel) {
    if (!isPoisoned) {
        return getColoredText(text, baseColor);
    }

    // Parse the base color if it's an RGB format
    QColor color;
    if (baseColor.startsWith("rgb")) {
        QStringList values = baseColor.mid(4, baseColor.length() - 5).split(',');
        if (values.size() == 3) {
            int r = values[0].toInt();
            int g = values[1].toInt();
            int b = values[2].toInt();
            color = QColor(r, g, b);
        }
    } else {
        color = QColor(baseColor);
    }

    int r = qMin(255, static_cast<int>(color.red() * (1 + poisonLevel * 0.5)));  // Increase red
    int g = color.green() * (1 - poisonLevel);  // Decrease green
    int b = qMin(255, static_cast<int>(color.blue() * (1 + poisonLevel * 0.5))); // Increase blue

    QString poisonColor = QString("rgb(%1,%2,%3)").arg(r).arg(g).arg(b);
    return QString("<span style='color: %1;'>%2</span>").arg(poisonColor, text);
}


void TextBasedView::startAttackAnimation() {
    attackAnimFrame = 0;
    attackAnimTimer->start(150);
}

void TextBasedView::updateAttackAnimation() {
    // Get the protagonist's direction
    Direction protagonistDir = model.getCurrentLevel()->getProtagonist()->getDirection();

    // Determine the frame based on the direction
    int targetFrame = 0;
    switch (protagonistDir) {
    case Direction::UP:
        targetFrame = 1;
        break;
    case Direction::DOWN:
        targetFrame = 2;
        break;
    case Direction::LEFT:
        targetFrame = 3;
        break;
    case Direction::RIGHT:
        targetFrame = 4;
        break;
    default:
        targetFrame = 0;
        break;
    }

    // Alternate between frame 0 and the target frame
    attackAnimFrame = (attackAnimFrame == 0) ? targetFrame : 0;

    // Redraw the world to reflect the new frame
    visualizeWorld();
}

void TextBasedView::startDeathAnimation(int x, int y) {
    deathAnimations.append({x, y, 0});
    if (!deathAnimTimer->isActive()) {
        deathAnimTimer->start(100);
    }
}

void TextBasedView::updateProtagonistAnimation() {
    protagonistAnimFrame = (protagonistAnimFrame + 1) % PROTAGONIST_FRAMES.size();
    visualizeWorld();
}

void TextBasedView::updateDeathAnimation() {
    bool hasActiveAnimations = false;

    for (auto& anim : deathAnimations) {
        anim.frame++;
        if (anim.frame < DEATH_FRAMES.size()) {
            hasActiveAnimations = true;
        }
    }

    if (!hasActiveAnimations) {
        deathAnimTimer->stop();
        deathAnimations.clear();
    }

    visualizeWorld();
}

void TextBasedView::onEnemyDeath() {
    auto enemy = qobject_cast<EnemyModel*>(sender());
    if (enemy) {
        startDeathAnimation(enemy->getXPos(), enemy->getYPos());
    }
}

QString TextBasedView::getAsciiForGrayValue(float value) const {
    // Explicit mappings for specific grayscale values
    static const QMap<int, QString> explicitMappings = {
        {30, " = "}, {100, " m "}, {128, " ▀ "}, {150, " . "}, {170, " ^ "}
    };

    // Convert float value to an integer grayscale value (0–255 range)
    int grayValue = static_cast<int>(value * 255);

    // Check if there's an explicit mapping for this grayscale value
    if (explicitMappings.contains(grayValue)) {
        return explicitMappings[grayValue];
    }

    // If no explicit mapping, fallback to general grayscale character mapping
    int index = static_cast<int>((1.0f - value) * (GRAYSCALE_CHARS.size() - 1));
    return GRAYSCALE_CHARS[qBound(0, index, GRAYSCALE_CHARS.size() - 1)];
}

void TextBasedView::onCurrentLevelChanged()
{
    auto level = model.getCurrentLevel();
    if (!level) return;

    auto currentProtagonist = level->getProtagonist();

    // Disconnect old protagonist signals
    if (currentProtagonist) {
        disconnect(currentProtagonist.get(), &ProtagonistModel::healthChanged, this, &TextBasedView::onHealthChanged);
    }

    // Update the protagonist reference and connect signals
    currentProtagonist = level->getProtagonist();
    if (currentProtagonist) {
        connect(currentProtagonist.get(), &ProtagonistModel::healthChanged, this, &TextBasedView::onHealthChanged);
    }

    visualizeWorld(); // Refresh the view for the new level
}

void TextBasedView::visualizeWorld()
{
    QString worldRepresentation = generateTextRepresentation();
    textOutput->setText(worldRepresentation);
}

QString TextBasedView::generateTextRepresentation() {
    // Reset grid with appropriate grayscale characters based on tile values
    grid = QVector<QVector<QString>>(GRID_HEIGHT, QVector<QString>(GRID_WIDTH));

    // Fill grid with grayscale characters based on tile values
    for (const auto& tile : model.getCurrentLevel()->getTileModels()) {
        int x = tile->getXPos();
        int y = tile->getYPos();
        if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
            grid[y][x] = getAsciiForGrayValue(tile->getValue());
        }
    }

    // Update protagonist position
    int protagonistX = model.getCurrentLevel()->getProtagonist()->getXPos();
    int protagonistY = model.getCurrentLevel()->getProtagonist()->getYPos();

    if (protagonistX >= 0 && protagonistX < GRID_WIDTH &&
        protagonistY >= 0 && protagonistY < GRID_HEIGHT) {
        QString protagonistSymbol;
        if (model.getCurrentLevel()->getProtagonist()->getStatus() == ProtagonistStatus::ATTACKING) {
            protagonistSymbol = ATTACK_FRAMES[attackAnimFrame];
        } else {
            protagonistSymbol = PROTAGONIST_FRAMES[protagonistAnimFrame];  // Use animated frame
        }

        // Apply different color when hit
        if (isProtagonistHit) {
            grid[protagonistY][protagonistX] = getColoredText(protagonistSymbol, "#FF0000"); // Red when hit
        } else if (isHealing) {
            grid[protagonistY][protagonistX] = getColoredText(protagonistSymbol, "#90EE90"); // Light green when healing
        } else {
            grid[protagonistY][protagonistX] = getColoredText(protagonistSymbol, "#FFFFFF"); // Normal white color
        }
    }

    // Update enemies
    for (const auto& enemy : model.getCurrentLevel()->getEnemies()) {
        int enemyX = enemy->getXPos();
        int enemyY = enemy->getYPos();

        if (enemyX >= 0 && enemyX < GRID_WIDTH &&
            enemyY >= 0 && enemyY < GRID_HEIGHT) {
            if (enemy->getDefeated()) {
                grid[enemyY][enemyX] = " † ";  // ASCII cross for defeated enemies
            } else if(dynamic_cast<PEnemyModel*>(enemy.get())) {
                grid[enemyY][enemyX] = " T ";
            } else if(dynamic_cast<ChasingEnemyModel*>(enemy.get())) {
                grid[enemyY][enemyX] = " X ";
            } else {
                grid[enemyY][enemyX] = " R ";
            }
        }
    }

    // Apply death animations
    for (const auto& anim : deathAnimations) {
        if (anim.frame < DEATH_FRAMES.size() &&
            anim.x >= 0 && anim.x < GRID_WIDTH &&
            anim.y >= 0 && anim.y < GRID_HEIGHT) {
            grid[anim.y][anim.x] = DEATH_FRAMES[anim.frame];
        }
    }

    // Update health packs
    for (const auto& healthPack : model.getCurrentLevel()->getHealthPacks()) {
        int healthX = healthPack->getXPos();
        int healthY = healthPack->getYPos();

        if (healthX >= 0 && healthX < GRID_WIDTH &&
            healthY >= 0 && healthY < GRID_HEIGHT) {
            grid[healthY][healthX] = " + ";  // Plus sign for health
        }
    }

    QString output;
    output = "<pre style='margin:0;'>";

    // Add top border
    output += "+" + QString(GRID_WIDTH * 3, '-') + "+\n";

    // Create a set of path coordinates for faster lookup
    std::set<std::pair<int, int>> pathSet;
    for (const auto& pos : pathTiles) {
        pathSet.insert(pos);
    }

    // When coloring tiles in the visualization loop, add path highlighting:
    for (int y = 0; y < GRID_HEIGHT; y++) {
        output += "|";
        for (int x = 0; x < GRID_WIDTH; x++) {
            QString cell = grid[y][x];
            auto tile = model.getCurrentLevel()->getTileAt(x, y);
            bool isPoisoned = tile.isPoisoned();
            float poisonLevel = tile.getPoisonLevel();

            // Check if current position is part of the path
            bool isPathTile = pathSet.find(std::make_pair(x, y)) != pathSet.end();

            // If it's a path tile and not a special character (protagonist, enemy, etc.)
            if (isPathTile && !cell.contains("<span")) {
                // Use a bright cyan color for path highlighting
                output += getColoredTextWithPoison(cell, "#00FFFF", isPoisoned, poisonLevel);
                continue;
            }

            if (cell == " T ") {
                output += getColoredTextWithPoison(cell, "#A020F0", isPoisoned, poisonLevel);
            } else if (cell == " X ") {
                output += getColoredTextWithPoison(cell, "#ADD8E6", isPoisoned, poisonLevel);
            } else if (cell == " R ") {
                output += getColoredTextWithPoison(cell, "#FF0000", isPoisoned, poisonLevel);
            } else if (cell == " † ") {
                output += getColoredTextWithPoison(cell, "#666666", isPoisoned, poisonLevel);
            } else if (cell == " + ") {
                output += getColoredTextWithPoison(cell, "#00FFFF", isPoisoned, poisonLevel);
            } else if (cell == " ^ ") {
                output += getColoredTextWithPoison(cell, "#00FF00", isPoisoned, poisonLevel);
            } else if (cell == " ~ ") {
                output += getColoredTextWithPoison(cell, "#0000FF", isPoisoned, poisonLevel);
            } else if (cell == " = ") {
                output += getColoredTextWithPoison(cell, "#B0B0B0", isPoisoned, poisonLevel);
            } else if (DEATH_FRAMES.contains(cell)) {
                output += getColoredTextWithPoison(cell, "#FFF00", isPoisoned, poisonLevel);
            } else if (cell == " m ") {
                output += getColoredTextWithPoison(cell, "#FFA500", isPoisoned, poisonLevel);
            } else if (cell == " * ") {
                output += getColoredTextWithPoison(cell, "#808080", isPoisoned, poisonLevel);
            } else if (cell == " . ") {
                output += getColoredTextWithPoison(cell, "#FFFF00", isPoisoned, poisonLevel);
            } else {
                // For grayscale tiles, use grayscale colors with poison effect
                float value = 0.0f;
                for (int i = 0; i < GRAYSCALE_CHARS.size(); ++i) {
                    if (cell == GRAYSCALE_CHARS[i]) {
                        value = 1.0f - (static_cast<float>(i) / (GRAYSCALE_CHARS.size() - 1));
                        break;
                    }
                }
                int grayValue = static_cast<int>(value * 255);
                QString color = QString("rgb(%1,%1,%1)").arg(grayValue);
                output += getColoredTextWithPoison(cell, color, isPoisoned, poisonLevel);
            }
        }
        output += "|\n";
    }

    // Add bottom border
    output += "+" + QString(GRID_WIDTH * 3, '-') + "+\n";
    output += "</pre>";

    return output;
}

void TextBasedView::protagonistStatusUpdated(ProtagonistStatus status) {
    if (status == ProtagonistStatus::ATTACKING) {
        qDebug() << "TBV was notified that protagonist is attacking!";
        startAttackAnimation();
    } else {
        attackAnimTimer->stop();
    }
    visualizeWorld();
}

void TextBasedView::onHealthChanged(int damage) {
    if (damage < 0) {
        qDebug() << "Protagonist grabbed a health pak and regained strength (+20hp)";
        if (model.getCurrentLevel()->getProtagonist()) {
            isHealing = true;
            healingAnimTimer->start(1000); // 1 second of healing animation
            visualizeWorld();
        }
    } else {
        qDebug() << "Protagonist was hit!";
        isProtagonistHit = true;
        protagonistHitTimer->start(300);
        visualizeWorld();
    }
}

TextBasedView::~TextBasedView() {
    if (protagonistAnimTimer) {
        protagonistAnimTimer->stop();
        delete protagonistAnimTimer;
    }

    if (healingAnimTimer) {
        healingAnimTimer->stop();
        delete healingAnimTimer;
    }

    if (protagonistHitTimer) {
        protagonistHitTimer->stop();
        delete protagonistHitTimer;
    }

    if(deathAnimTimer) {
        deathAnimTimer->stop();
        delete deathAnimTimer;
    }
}

void TextBasedView::updatePathTiles(const std::vector<std::pair<int, int>>& newPath) {
    pathTiles = newPath;
    visualizeWorld();
}
