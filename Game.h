#pragma once

#include <optional>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Board.h"

enum class GameState
{
    StartScreen,
    NameInput,
    Transitioning,
    Playing,
    GameOver
};

class Game
{
public:
    Game();
    void run();

private:
    void processEvents();
    void handleMouseClick(const sf::Vector2i& pixelPos);
    void handleTextInput(unsigned int unicode);
    void switchTurn();
    void updateStatusText();
    void render();
    void checkForGameOver();
    std::vector<sf::Vector2i> currentHighlightSquares() const;

    sf::RenderWindow m_window;
    Board m_board;
    PieceColor m_currentPlayer;
    PieceColor m_winner{PieceColor::First};
    GameState m_state = GameState::StartScreen;

    std::optional<sf::Vector2i> m_selectedSquare;
    std::vector<Board::Move> m_currentMoves;
    bool m_forcedCaptureChain = false;
    sf::Vector2i m_chainPiece{-1, -1};

    sf::Font m_font;
    std::optional<sf::Text> m_turnText;
    bool m_fontLoaded = false;
    std::optional<sf::Text> m_titleText;

    float m_cellSize = 100.f;

    bool m_gameOver = false;
    std::string m_gameOverMessage;

    sf::RectangleShape m_startButton;
    std::optional<sf::Text> m_startButtonText;

    sf::RectangleShape m_rematchButton;
    std::optional<sf::Text> m_rematchButtonText;

    float m_transitionAlpha = 0.f;
    float m_transitionSpeed = 3.f;
    sf::RectangleShape m_transitionOverlay;

    std::string m_playerFirstName = "First";
    std::string m_playerSecondName = "Second";
    std::string m_currentInputName;
    bool m_inputtingFirstName = true;
    sf::RectangleShape m_nameInputBox;
    std::optional<sf::Text> m_nameInputLabel;
    std::optional<sf::Text> m_nameInputText;
    sf::RectangleShape m_nameInputButton;
    std::optional<sf::Text> m_nameInputButtonText;
};
