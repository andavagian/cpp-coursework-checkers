#include "Game.h"

#include <iostream>

namespace
{
constexpr unsigned WINDOW_SIZE = 800;
constexpr char FONT_PATH[] = "assets/DejaVuSans.ttf";
constexpr float TRANSITION_DURATION = 1.0f;
}

Game::Game()
    : m_window(sf::VideoMode({WINDOW_SIZE, WINDOW_SIZE}), "SFML Checkers")
    , m_currentPlayer(PieceColor::First)
{
    m_window.setFramerateLimit(60);
    m_cellSize = static_cast<float>(WINDOW_SIZE) / static_cast<float>(Board::SIZE);

    if (m_font.openFromFile(FONT_PATH))
    {
        m_fontLoaded = true;
        m_turnText.emplace(m_font);
        m_turnText->setCharacterSize(28);
        m_turnText->setFillColor(sf::Color(101, 67, 33));
        m_turnText->setOutlineColor(sf::Color(245, 230, 200));
        m_turnText->setOutlineThickness(2.f);
        m_turnText->setPosition({10.f, 10.f});

        m_titleText.emplace(m_font, "CHECKERS", 72);
        sf::FloatRect titleBounds = m_titleText->getLocalBounds();
        m_titleText->setOrigin({titleBounds.size.x / 2.f, titleBounds.size.y / 2.f});
        m_titleText->setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE * 0.35f});
        m_titleText->setFillColor(sf::Color(101, 67, 33));
        m_titleText->setLetterSpacing(2.f);

        m_startButtonText.emplace(m_font, "START GAME", 28);
        sf::FloatRect sb = m_startButtonText->getLocalBounds();
        m_startButtonText->setOrigin({sb.size.x / 2.f, sb.size.y / 2.f});
        m_startButtonText->setFillColor(sf::Color(245, 230, 200));
        m_startButtonText->setLetterSpacing(1.5f);

        m_rematchButtonText.emplace(m_font, "Rematch", 32);
        sf::FloatRect rb = m_rematchButtonText->getLocalBounds();
        m_rematchButtonText->setOrigin({rb.size.x / 2.f, rb.size.y / 2.f});

        m_nameInputLabel.emplace(m_font, "Enter First Player Name:", 32);
        sf::FloatRect labelBounds = m_nameInputLabel->getLocalBounds();
        m_nameInputLabel->setOrigin({labelBounds.size.x / 2.f, labelBounds.size.y / 2.f});
        m_nameInputLabel->setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE * 0.35f});
        m_nameInputLabel->setFillColor(sf::Color(101, 67, 33));

        m_nameInputText.emplace(m_font, "", 28);
        m_nameInputText->setFillColor(sf::Color(101, 67, 33));
        m_nameInputText->setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE * 0.45f});

        m_nameInputButtonText.emplace(m_font, "CONTINUE", 24);
        sf::FloatRect btnBounds = m_nameInputButtonText->getLocalBounds();
        m_nameInputButtonText->setOrigin({btnBounds.size.x / 2.f, btnBounds.size.y / 2.f});
        m_nameInputButtonText->setFillColor(sf::Color(245, 230, 200));
        m_nameInputButtonText->setLetterSpacing(1.2f);
    }
    else
    {
        std::cerr << "Warning: Unable to load font at " << FONT_PATH
                  << ". Turn text will be disabled.\n";
    }

    updateStatusText();

    m_startButton.setSize({220.f, 60.f});
    m_startButton.setFillColor(sf::Color(101, 67, 33));
    m_startButton.setOutlineColor(sf::Color(80, 50, 30));
    m_startButton.setOutlineThickness(2.f);
    m_startButton.setOrigin({110.f, 30.f});
    m_startButton.setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE * 0.55f});

    m_transitionOverlay.setSize({static_cast<float>(WINDOW_SIZE), static_cast<float>(WINDOW_SIZE)});
    m_transitionOverlay.setFillColor(sf::Color(0, 0, 0, 0));

    m_rematchButton.setSize({280.f, 70.f});
    m_rematchButton.setFillColor(sf::Color(46, 139, 87));
    m_rematchButton.setOutlineColor(sf::Color::White);
    m_rematchButton.setOutlineThickness(3.f);
    m_rematchButton.setOrigin({140.f, 35.f});
    m_rematchButton.setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE * 0.65f});

    m_nameInputBox.setSize({400.f, 50.f});
    m_nameInputBox.setFillColor(sf::Color(245, 230, 200));
    m_nameInputBox.setOutlineColor(sf::Color(101, 67, 33));
    m_nameInputBox.setOutlineThickness(2.f);
    m_nameInputBox.setOrigin({200.f, 25.f});
    m_nameInputBox.setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE * 0.45f});

    m_nameInputButton.setSize({180.f, 50.f});
    m_nameInputButton.setFillColor(sf::Color(101, 67, 33));
    m_nameInputButton.setOutlineColor(sf::Color(80, 50, 30));
    m_nameInputButton.setOutlineThickness(2.f);
    m_nameInputButton.setOrigin({90.f, 25.f});
    m_nameInputButton.setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE * 0.6f});

    m_currentInputName = "";
    m_inputtingFirstName = true;
}

void Game::run()
{
    sf::Clock clock;
    while (m_window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        processEvents();
        
        if (m_state == GameState::Transitioning)
        {
            m_transitionAlpha += m_transitionSpeed * deltaTime * 255.f;
            if (m_transitionAlpha >= 255.f)
            {
                m_transitionAlpha = 255.f;
                m_state = GameState::Playing;
            }
            m_transitionOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<unsigned char>(m_transitionAlpha)));
        }
        else if (m_state == GameState::Playing && m_transitionAlpha > 0.f)
        {
            m_transitionAlpha -= m_transitionSpeed * deltaTime * 255.f;
            if (m_transitionAlpha < 0.f)
            {
                m_transitionAlpha = 0.f;
            }
            m_transitionOverlay.setFillColor(sf::Color(0, 0, 0, static_cast<unsigned char>(m_transitionAlpha)));
        }
        
        render();
    }
}

void Game::processEvents()
{
    while (const std::optional<sf::Event> event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
        }
        else if (const auto* textEntered = event->getIf<sf::Event::TextEntered>())
        {
            if (m_state == GameState::NameInput)
            {
                unsigned int unicode = textEntered->unicode;
                if (unicode == 13 || unicode == 10)
                {
                    handleMouseClick({0, 0});
                }
                else if (unicode == 8)
                {
                    if (!m_currentInputName.empty())
                    {
                        m_currentInputName.pop_back();
                        if (m_nameInputText)
                        {
                            m_nameInputText->setString(m_currentInputName);
                            sf::FloatRect textBounds = m_nameInputText->getLocalBounds();
                            m_nameInputText->setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
                        }
                    }
                }
                else
                {
                    handleTextInput(unicode);
                }
            }
        }
        else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePressed->button == sf::Mouse::Button::Left)
            {
                handleMouseClick(mousePressed->position);
            }
        }
        else if (const auto* mouseMoved = event->getIf<sf::Event::MouseMoved>())
        {
            if (m_state == GameState::StartScreen)
            {
                sf::Vector2f p(static_cast<float>(mouseMoved->position.x), static_cast<float>(mouseMoved->position.y));
                if (m_startButton.getGlobalBounds().contains(p))
                {
                    m_startButton.setFillColor(sf::Color(120, 80, 50));
                }
                else
                {
                    m_startButton.setFillColor(sf::Color(101, 67, 33));
                }
            }
            else if (m_state == GameState::NameInput)
            {
                sf::Vector2f p(static_cast<float>(mouseMoved->position.x), static_cast<float>(mouseMoved->position.y));
                if (m_nameInputButton.getGlobalBounds().contains(p))
                {
                    m_nameInputButton.setFillColor(sf::Color(120, 80, 50));
                }
                else
                {
                    m_nameInputButton.setFillColor(sf::Color(101, 67, 33));
                }
            }
        }
    }
}

void Game::handleTextInput(unsigned int unicode)
{
    if (unicode >= 32 && unicode < 127 && m_currentInputName.length() < 20)
    {
        m_currentInputName += static_cast<char>(unicode);
        if (m_nameInputText)
        {
            m_nameInputText->setString(m_currentInputName);
            sf::FloatRect textBounds = m_nameInputText->getLocalBounds();
            m_nameInputText->setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
        }
    }
}

void Game::handleMouseClick(const sf::Vector2i& pixelPos)
{
    if (m_state == GameState::StartScreen)
    {
        sf::Vector2f p(static_cast<float>(pixelPos.x), static_cast<float>(pixelPos.y));
        if (m_startButton.getGlobalBounds().contains(p))
        {
            m_state = GameState::NameInput;
            m_inputtingFirstName = true;
            m_currentInputName = "";
            if (m_nameInputLabel)
            {
                m_nameInputLabel->setString("Enter First Player Name:");
                sf::FloatRect labelBounds = m_nameInputLabel->getLocalBounds();
                m_nameInputLabel->setOrigin({labelBounds.size.x / 2.f, labelBounds.size.y / 2.f});
            }
            if (m_nameInputText)
            {
                m_nameInputText->setString("");
            }
            return;
        }
        return;
    }
    if (m_state == GameState::NameInput)
    {
        sf::Vector2f p(static_cast<float>(pixelPos.x), static_cast<float>(pixelPos.y));
        if (m_nameInputButton.getGlobalBounds().contains(p))
        {
            if (m_inputtingFirstName)
            {
                m_playerFirstName = m_currentInputName.empty() ? "First" : m_currentInputName;
                m_inputtingFirstName = false;
                m_currentInputName = "";
                if (m_nameInputLabel)
                {
                    m_nameInputLabel->setString("Enter Second Player Name:");
                    sf::FloatRect labelBounds = m_nameInputLabel->getLocalBounds();
                    m_nameInputLabel->setOrigin({labelBounds.size.x / 2.f, labelBounds.size.y / 2.f});
                }
                if (m_nameInputText)
                {
                    m_nameInputText->setString("");
                }
            }
            else
            {
                m_playerSecondName = m_currentInputName.empty() ? "Second" : m_currentInputName;
                m_state = GameState::Transitioning;
                m_transitionAlpha = 0.f;
            }
            return;
        }
        return;
    }
    if (m_state == GameState::GameOver)
    {
        sf::Vector2f p(static_cast<float>(pixelPos.x), static_cast<float>(pixelPos.y));
        if (m_rematchButton.getGlobalBounds().contains(p))
        {
            m_board.reset();
            m_currentPlayer = PieceColor::First;
            m_selectedSquare.reset();
            m_currentMoves.clear();
            m_forcedCaptureChain = false;
            m_chainPiece = {-1, -1};
            m_gameOver = false;
            m_state = GameState::Playing;
            updateStatusText();
        }
        return;
    }

    if (m_gameOver || m_state != GameState::Playing)
    {
        return;
    }

    const int col = static_cast<int>(pixelPos.x / m_cellSize);
    const int row = static_cast<int>(pixelPos.y / m_cellSize);
    sf::Vector2i boardPos(row, col);

    if (!Board::isInside(boardPos))
    {
        return;
    }

    if (m_selectedSquare)
    {
        for (const auto& move : m_currentMoves)
        {
            if (move.to == boardPos)
            {
                if (!m_board.applyMove(move))
                {
                    return;
                }

                const bool performedCapture = move.isCapture;

                m_selectedSquare.reset();
                m_currentMoves.clear();

                checkForGameOver();
                if (m_gameOver)
                {
                    updateStatusText();
                    return;
                }

                if (performedCapture)
                {
                    auto followUp = m_board.getMovesForPiece(move.to, true);
                    if (!followUp.empty())
                    {
                        m_selectedSquare = move.to;
                        m_currentMoves = followUp;
                        m_forcedCaptureChain = true;
                        m_chainPiece = move.to;
                        updateStatusText();
                        return;
                    }
                }

                m_forcedCaptureChain = false;
                m_chainPiece = {-1, -1};

                PieceColor justPlayed = m_currentPlayer;
                switchTurn();

                const int opponentPieces = m_board.countPieces(m_currentPlayer);
                const bool opponentStuck = !m_board.playerHasMoves(m_currentPlayer);
                if (opponentPieces == 0 || opponentStuck)
                {
                    m_gameOver = true;
                    m_state = GameState::GameOver;
                    const std::string winner = (justPlayed == PieceColor::First ? m_playerFirstName : m_playerSecondName);
                    if (opponentPieces == 0)
                    {
                        m_gameOverMessage = winner + " wins! Opponent has no pieces left.";
                    }
                    else
                    {
                        m_gameOverMessage = winner + " wins! Opponent has no legal moves.";
                    }
                    m_winner = justPlayed;
                }
                updateStatusText();
                return;
            }
        }
    }

    if (m_forcedCaptureChain && (!m_selectedSquare || boardPos != m_chainPiece))
    {
        return;
    }

    const Piece* piece = m_board.pieceAt(boardPos);
    if (!piece || piece->getColor() != m_currentPlayer)
    {
        return;
    }

    const bool capturesMandatory = m_board.hasCaptureMoves(m_currentPlayer);
    auto moves = m_board.getMovesForPiece(boardPos, capturesMandatory);
    if (moves.empty())
    {
        return;
    }

    m_selectedSquare = boardPos;
    m_currentMoves = moves;
    m_chainPiece = boardPos;
    updateStatusText();
}

void Game::switchTurn()
{
    m_currentPlayer = (m_currentPlayer == PieceColor::First) ? PieceColor::Second : PieceColor::First;
}

void Game::updateStatusText()
{
    if (!m_fontLoaded || !m_turnText)
    {
        return;
    }

    if (m_gameOver || m_state != GameState::Playing)
    {
        m_turnText->setString("");
        return;
    }

    std::string text = "Turn: ";
    text += (m_currentPlayer == PieceColor::First ? m_playerFirstName : m_playerSecondName);
    if (m_forcedCaptureChain)
    {
        text += " (continue jumping)";
    }
    else if (!m_currentMoves.empty())
    {
        text += " (piece selected)";
    }
    m_turnText->setString(text);
}

void Game::render()
{
    m_window.clear(sf::Color(240, 235, 220));
    
    if (m_state == GameState::StartScreen)
    {
        sf::RectangleShape background(sf::Vector2f{static_cast<float>(WINDOW_SIZE), static_cast<float>(WINDOW_SIZE)});
        background.setFillColor(sf::Color(240, 235, 220));
        m_window.draw(background);
        
        if (m_fontLoaded && m_titleText)
        {
            m_window.draw(*m_titleText);
        }
        m_window.draw(m_startButton);
        if (m_startButtonText)
        {
            auto pos = m_startButton.getPosition();
            m_startButtonText->setPosition(pos);
            m_window.draw(*m_startButtonText);
        }
    }
    else if (m_state == GameState::NameInput)
    {
        sf::RectangleShape background(sf::Vector2f{static_cast<float>(WINDOW_SIZE), static_cast<float>(WINDOW_SIZE)});
        background.setFillColor(sf::Color(240, 235, 220));
        m_window.draw(background);
        
        if (m_fontLoaded && m_nameInputLabel)
        {
            m_window.draw(*m_nameInputLabel);
        }
        
        m_window.draw(m_nameInputBox);
        
        if (m_nameInputText)
        {
            m_window.draw(*m_nameInputText);
        }
        
        m_window.draw(m_nameInputButton);
        if (m_nameInputButtonText)
        {
            auto pos = m_nameInputButton.getPosition();
            m_nameInputButtonText->setPosition(pos);
            m_window.draw(*m_nameInputButtonText);
        }
    }
    else if (m_state == GameState::Transitioning || m_state == GameState::Playing)
    {
        m_board.draw(m_window, m_cellSize, m_selectedSquare, currentHighlightSquares());

        if (m_fontLoaded && m_turnText)
        {
            m_window.draw(*m_turnText);
        }
        
        if (m_transitionAlpha > 0.f)
        {
            m_window.draw(m_transitionOverlay);
        }
    }
    else if (m_state == GameState::GameOver)
    {
        m_board.draw(m_window, m_cellSize, std::nullopt, {});
        
        sf::RectangleShape overlay(sf::Vector2f{static_cast<float>(WINDOW_SIZE), static_cast<float>(WINDOW_SIZE)});
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        m_window.draw(overlay);
        
        if (m_fontLoaded)
        {
            sf::Text message(m_font, m_gameOverMessage, 36);
            sf::FloatRect mb = message.getLocalBounds();
            message.setOrigin({mb.size.x / 2.f, mb.size.y / 2.f});
            message.setPosition({WINDOW_SIZE / 2.f, WINDOW_SIZE / 3.f});
            message.setFillColor(sf::Color(245, 230, 200));
            m_window.draw(message);
        }
        m_window.draw(m_rematchButton);
        if (m_rematchButtonText)
        {
            auto pos = m_rematchButton.getPosition();
            m_rematchButtonText->setPosition(pos);
            m_window.draw(*m_rematchButtonText);
        }
    }

    m_window.display();
}

void Game::checkForGameOver()
{
    if (m_board.countPieces(PieceColor::First) == 0)
    {
        m_gameOver = true;
        m_gameOverMessage = m_playerSecondName + " wins!";
        m_winner = PieceColor::Second;
        m_state = GameState::GameOver;
    }
    else if (m_board.countPieces(PieceColor::Second) == 0)
    {
        m_gameOver = true;
        m_gameOverMessage = m_playerFirstName + " wins!";
        m_winner = PieceColor::First;
        m_state = GameState::GameOver;
    }
}

std::vector<sf::Vector2i> Game::currentHighlightSquares() const
{
    std::vector<sf::Vector2i> highlights;
    for (const auto& move : m_currentMoves)
    {
        highlights.push_back(move.to);
    }
    return highlights;
}
