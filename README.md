# Warzone (COMP 345 Project)

## Project Description

Warzone is a C++ implementation of a Risk-like strategy board game where 2-6 players compete to conquer territories on a world map. The game features multiple phases including map setup, player reinforcement, order issuing, and order execution. Players can employ different strategies (Human, Aggressive, Benevolent, Neutral, Cheater) to achieve world domination.

The implementation includes a complete game engine with state management, command processing (console and file-based), map loading and validation, card management, order execution system, player strategies, and a logging/observer system for tracking all game actions.

The project supports both single games and tournament mode, where multiple AI strategies can compete across different maps with configurable game parameters.

## Game Architecture

### Core Components

The game is built using object-oriented principles with clear separation of concerns:

- **Game Engine**: Central orchestrator managing game states, transitions, and the main game loop
- **Command Processing**: Handles user input validation and command execution through console or file
- **Map System**: Manages territories, continents, and their connections with validation
- **Player Management**: Tracks players, their territories, armies, cards, and strategies
- **Order System**: Implements various order types (Deploy, Advance, Bomb, Airlift, etc.)
- **Card Mechanism**: Provides special abilities through a deck/hand system
- **Strategy Pattern**: Enables different AI behaviors (Aggressive, Benevolent, Neutral, Cheater, Human)
- **Observer Pattern**: Logs all significant game events to file for analysis

### Game Flow

1. **Startup Phase**: Load and validate map → Add players → Assign territories → Distribute initial armies
2. **Reinforcement Phase**: Calculate and distribute reinforcement armies based on territories and continents
3. **Issue Orders Phase**: Players issue orders (deploy, advance, play cards) based on their strategy
4. **Execute Orders Phase**: Process all orders in priority sequence (deploy → airlift → blockade → etc.)
5. **Win Condition Check**: Determine if a player controls all territories

The game continues through these phases until one player achieves total domination or the maximum turn limit is reached.

## Design Patterns

The project implements several design patterns to ensure maintainability, extensibility, and clean code architecture:

### 1. Strategy Pattern
**Location**: `PlayerStrategies` package

The Strategy pattern is used to encapsulate different AI behaviors. The `PlayerStrategy` abstract class defines the interface for issuing orders and selecting territories. Five concrete strategies implement different play styles:
- `HumanPlayerStrategy` - Interactive user input
- `AggressivePlayerStrategy` - Offensive gameplay
- `BenevolentPlayerStrategy` - Defensive gameplay
- `NeutralPlayerStrategy` - Passive until provoked
- `CheaterPlayerStrategy` - Automatic conquest

This allows players to change behavior at runtime and makes adding new strategies straightforward without modifying existing code.

### 2. Observer Pattern
**Location**: `LoggingObserver` package

The Observer pattern enables automatic logging of game events. The `Subject` class (inherited by `GameEngine`, `Order`, `OrdersList`, `Command`, `CommandProcessor`) maintains a list of observers and notifies them when significant events occur. The `LogObserver` concrete observer writes these events to `gamelog.txt`. The `ILoggable` interface ensures all observable objects can produce string representations for logging.

This decouples the game logic from logging concerns and allows multiple observers to track different aspects of gameplay.

### 3. Adapter Pattern
**Location**: `CommandProcessor` package

The Adapter pattern is implemented through `FileCommandProcessorAdapter`, which adapts file input to work with the `CommandProcessor` interface (`ICommandProcessor`). This allows the game to seamlessly accept commands from either console input or file input using the same processing logic.

### 4. State Pattern
**Location**: `GameEngine` package

The State pattern manages game flow through different phases. The `State` class encapsulates state-specific behavior, and the `GameEngine` maintains the current state (`StateType` enum: Start, MapLoaded, MapValidated, PlayersAdded, AssignReinforcement, IssueOrders, ExecuteOrders, Win). Each state defines which commands are valid, ensuring proper game progression and preventing invalid actions.

### 5. Command Pattern
**Location**: `CommandProcessor` package

The Command pattern encapsulates user requests as `Command` objects. Each command stores the command text, its effect after execution, and its type (`CommandType`). This enables command history tracking, validation before execution, and separation of command invocation from command implementation.

## Classes Overview

### GameEngine Package
- **GameEngine**: Main game controller managing states, phases, and game flow
- **State**: Represents current game state (Start, Map Loaded, Map Validated, Players Added, etc.)

### Map Package
- **Map**: Container for continents and territories with connectivity validation
- **Continent**: Group of territories with reinforcement bonus
- **Territory**: Individual map location with armies, owner, and adjacent territories
- **MapLoader**: Parses and loads .map files into Map objects

### Player Package
- **Player**: Represents a game participant with territories, cards, orders, and strategy
- **Hand**: Collection of cards held by a player
- **Card**: Provides special abilities when played (bomb, reinforcement, blockade, airlift, diplomacy)
- **Deck**: Central card repository for drawing and returning cards

### PlayerStrategies Package
- **PlayerStrategy** (Abstract): Interface for AI behavior
- **HumanPlayerStrategy**: Interactive player requiring user input
- **AggressivePlayerStrategy**: Focuses on strongest territory and attacks continuously
- **BenevolentPlayerStrategy**: Protects weakest territories, never attacks
- **NeutralPlayerStrategy**: Passive until attacked, then becomes aggressive
- **CheaterPlayerStrategy**: Automatically conquers all adjacent enemy territories

### Orders Package
- **Order** (Abstract): Base class for all orders with validate() and execute()
- **Deploy**: Places armies on owned territory
- **Advance**: Moves armies between territories (move or attack)
- **Bomb**: Destroys half the armies on target territory
- **Blockade**: Converts territory to neutral with triple armies
- **Airlift**: Moves armies across non-adjacent territories
- **Negotiate**: Prevents attacks between two players for one turn
- **OrdersList**: Priority queue managing execution order

### Cards Package
- **Card**: Represents a special ability card (5 types)
- **Deck**: Manages the draw pile
- **Hand**: Player's current cards

### CommandProcessor Package
- **CommandProcessor**: Reads and validates console commands
- **FileCommandProcessorAdapter**: Reads commands from file
- **Command**: Encapsulates a command with its effect
- **Tournament**: Configuration for multi-game tournaments

### LoggingObserver Package
- **ILoggable**: Interface for objects that can be logged
- **Subject**: Manages observer attachments
- **Observer**: Abstract observer class
- **LogObserver**: Writes game events to gamelog.txt

### Utils Package
- **Utils**: Helper functions and constants (SEPARATOR_LINE, etc.)
- **GameTypes**: Enumerations for StateType, CommandType, CardType, OrderType

## Requirements
- [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake)
- [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

## VS Code setup

1. Clone repo
```bash
git clone https://github.com/comp345proj/Warzone.git
```

2. Make `.vscode` directory inside root

2. Copy json files from `config` to `.vscode`

3. From `Run and Debug` tab, run the `Warzone (Mac/Windows)` depending on your machine

## Contributors
- Mohammad Alshikh
- Gevorg Alaverdyan
- Yusuf Chahal
- Christian Adu
- Jack Hannan