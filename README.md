# Checkers Game

A fully functional checkers game implementation in C++ using SFML for graphics and user interaction.

## Project Overview

This project implements a classic checkers (draughts) game with a graphical user interface. The game follows standard checkers rules including forced captures, king promotion, and capture chains. Players take turns moving pieces diagonally on an 8x8 board, with the goal of capturing all opponent pieces or blocking their moves.

## Technologies Used

- **C++20**: Modern C++ features including `std::optional`, structured bindings, and constexpr
- **SFML (Simple and Fast Multimedia Library)**: Graphics rendering, window management, and event handling
- **Standard Library**: STL containers (`std::array`, `std::vector`, `std::optional`) for data management

## Algorithms and Data Structures

### Board Representation

- **2D Array Grid**: The board is represented as an 8x8 grid using `std::array<std::array<std::optional<Piece>, SIZE>, SIZE>`
- **Optional Pieces**: Each cell uses `std::optional<Piece>` to represent either an empty square or a piece

### Move Generation Algorithm

The move generation uses a **directional search algorithm**:

1. **Direction Calculation**: For each piece, valid move directions are calculated:

   - Regular pieces: Forward diagonals only (2 directions)
   - Kings: All four diagonals (4 directions)

2. **Move Validation**: For each direction:

   - Check for empty squares (non-capture moves)
   - Check for enemy pieces followed by empty squares (capture moves)
   - For kings: Continue sliding along the diagonal until blocked
   - For regular pieces: Stop after one square or one jump

3. **Capture Priority**: The algorithm prioritizes capture moves when available, enforcing the "forced capture" rule

### Game Logic Algorithms

- **Forced Capture Enforcement**: Before allowing any move, the system checks if capture moves exist and only allows captures
- **Capture Chain Detection**: After a capture, the system checks if the same piece can capture again, forcing continuation
- **King Promotion**: Automatically promotes pieces reaching the opposite end of the board
- **Win Condition Detection**: Checks for two conditions:
  - No pieces remaining for a player
  - No legal moves available for a player

### State Management

- **Game State Machine**: Uses an enum-based state machine (`StartScreen`, `Playing`, `GameOver`)
- **Turn Management**: Alternates between Red and Black players
- **Selection System**: Tracks selected piece and valid moves for visual feedback

## File Structure

### `main.cpp`

Entry point of the application. Creates a `Game` instance and starts the main game loop.

### `Game.h` / `Game.cpp`

**Game Class**: Manages the overall game flow and user interface

- Handles SFML window creation and event processing
- Translates mouse clicks to board coordinates
- Manages game states (start screen, playing, game over)
- Enforces turn order and capture chain rules
- Renders UI elements (buttons, text, board)
- Detects win conditions

### `Board.h` / `Board.cpp`

**Board Class**: Core game logic and board representation

- Stores all pieces in a 2D grid structure
- Generates legal moves for pieces and players
- Validates and applies moves
- Handles piece captures and king promotion
- Renders the checkerboard and pieces using SFML
- Provides board boundary checking utilities

### `Piece.h` / `Piece.cpp`

**Piece Class**: Represents individual checker pieces

- Stores piece color (Red or Black)
- Tracks king status
- Provides methods for promotion and status queries

## Key Features

1. **Standard Checkers Rules**:

   - Diagonal movement only
   - Forced captures when available
   - Capture chains (must continue jumping if possible)
   - King promotion at opposite end
   - Kings can move in all four diagonal directions

2. **User Interface**:

   - Visual board with alternating dark/light squares
   - Piece selection highlighting
   - Valid move indicators
   - Turn indicator text
   - Start screen and rematch functionality
   - Game over screen with winner announcement

3. **Input Handling**:
   - Mouse click detection
   - Board coordinate conversion
   - Piece selection and move execution
   - UI button interactions

## Building and Running

### Prerequisites

- C++20 compatible compiler (clang++ or g++)
- SFML library installed
- DejaVu Sans font file at `assets/DejaVuSans.ttf`

### Build Command (macOS with Homebrew)

```bash
clang++ -std=c++20 main.cpp Game.cpp Board.cpp Piece.cpp \
    -I/opt/homebrew/include -L/opt/homebrew/lib \
    -lsfml-graphics -lsfml-window -lsfml-system -o checkers
```

### Build Command (Linux)

```bash
g++ -std=c++20 main.cpp Game.cpp Board.cpp Piece.cpp \
    -lsfml-graphics -lsfml-window -lsfml-system -o checkers
```

### Running

```bash
./checkers
```

## Game Rules Implementation

- **Movement**: Regular pieces move forward diagonally one square; kings move diagonally any number of squares
- **Captures**: Pieces jump over opponent pieces diagonally, landing on the square immediately beyond
- **Forced Captures**: If a capture is available, it must be taken
- **Capture Chains**: After capturing, if the same piece can capture again, the player must continue
- **King Promotion**: Regular pieces reaching the opposite end row are automatically promoted to kings
- **Win Conditions**: A player wins when the opponent has no pieces left or no legal moves available

## Presentation Notes

### Technical Highlights

- Object-oriented design with clear separation of concerns
- Efficient move generation using directional search
- Modern C++ features (optional types, constexpr, range-based loops)
- Clean state management with enum-based state machine
- Responsive UI with visual feedback for user actions

### Algorithm Complexity

- Move generation: O(d × n) where d is directions (2-4) and n is board size (8)
- Move validation: O(1) for boundary checks, O(d) for direction validation
- Board rendering: O(n²) for iterating through all squares

### Design Patterns

- **Model-View-Controller**: Board (model), Game (controller), SFML rendering (view)
- **State Pattern**: Game state management with enum-based states
- **Command Pattern**: Move structure encapsulates move data

## Future Enhancements (Optional)

- AI opponent using minimax algorithm
- Move history and undo functionality
- Network multiplayer support
- Three-fold repetition draw detection
- Move notation recording
