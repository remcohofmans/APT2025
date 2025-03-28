# Tactical Adventure Game

A 2D tactical adventure game built with Qt, featuring both graphical and text-based visualization modes. Control a protagonist through procedurally generated levels, battle enemies, and manage resources like health and energy.

## Features

- **Dual Visualization Modes**: Switch between graphical and text-based views
- **Command System**: Control the game through keyboard or commands
- **Pathfinding**: Smart A* pathfinding for character movement
- **Enemy Types**: Different enemy types with unique behaviors:
  - Standard Enemies
  - Chasing Enemies that actively pursue the protagonist
  - Poison Enemies that contaminate surrounding tiles
- **Resource Management**: Manage health and energy during gameplay
- **Combat System**: Engage in tactical combat with enemies
- **Multiple Levels**: Navigate through different game levels with increasing difficulty

## Controls

### Keyboard Controls
- **W/Up Arrow**: Move up
- **A/Left Arrow**: Move left
- **S/Down Arrow**: Move down
- **D/Right Arrow**: Move right

### Command Interface
Type commands in the input box:
- `goto x y`: Move to specific coordinates
- `attack`: Attack the nearest enemy
- `heal`: Find and move to the nearest health pack
- `help`: Display all available commands

### Mouse Controls
- **Click on a tile**: Move to that location

## Game Mechanics

### Protagonist
- **Health**: Decreases when taking damage from enemies or poison. Game over when health reaches zero.
- **Energy**: Consumed when moving across tiles. Higher difficulty terrain costs more energy.
- **Status**: Can be in different states (idle, walking, attacking, hurt, defeated)

### Enemies
- **Standard Enemies**: Static enemies that attack when approached
- **Chasing Enemies**: Actively pursue the protagonist when within range
- **Poison Enemies**: Create poisonous areas affecting nearby tiles and damaging the protagonist

### Tiles and Environment
- Different grayscale values represent different terrain types
- Some tiles are impassable (walls, water)
- Poisoned tiles damage the protagonist when stepped on
- Door tiles allow transition between different levels

### Combat
- Combat is initiated when moving into an enemy's tile
- Protagonist automatically attacks the enemy
- Combat continues until either the enemy is defeated or the player moves away
- Protagonist's health decreases during combat based on enemy strength

### Health Packs
- Collect health packs to restore health
- Health packs appear as '+' symbols in text mode or special sprites in graphical mode

## Technical Information

### Architecture
The game follows a Model-View-Controller (MVC) architecture:
- **Model**: Game state, levels, entities
- **View**: Graphical and text-based visualizations
- **Controller**: Input handling, command processing

### Key Components
- **GameModel**: Singleton managing the game state
- **Level**: Manages level-specific data and entities
- **LevelManager**: Handles level loading and transitions
- **CollisionHandler**: Manages entity collisions and combat
- **PathFinder**: Implements A* pathfinding for navigation
- **AnimationController**: Manages sprite animations
- **CommandRegistry**: Processes and executes text commands

### Building from Source

#### Prerequisites
- Qt 6.0+ (Qt 5.15+ should also work)
- C++20 compatible compiler

#### Build Instructions
1. Clone the repository
2. Open `Game.pro` in Qt Creator
3. Configure the project for your platform and adjust paths
4. Build and run

## Development Notes

### Adding New Enemies
To add new enemy types:
1. Create a new class inheriting from `EnemyModel`
2. Implement behavior in the new class
3. Add rendering logic in `EntityRenderer`

### Adding New Commands
To add new commands:
1. Create a new class inheriting from `Command`
2. Implement the required functions
3. Register the command in `InputController::initializeCommands()`

### Level Creation
Levels are based on grayscale PNG images:
- Black (0): Impassable terrain
- White (255): Open terrain
- Gray values: Various terrain types with different movement costs

## Credits

Created as a graded project for the Advanced Programming Techniques course at KUL.
