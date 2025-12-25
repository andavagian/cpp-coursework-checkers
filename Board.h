#pragma once

#include <array>
#include <optional>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Piece.h"

class Board
{
public:
    struct Move
    {
        sf::Vector2i from{};
        sf::Vector2i to{};
        bool isCapture = false;
        sf::Vector2i captured{-1, -1};
    };

    static constexpr int SIZE = 8;

    Board();
    void reset();
    void draw(sf::RenderWindow& window,
              float cellSize,
              const std::optional<sf::Vector2i>& selected,
              const std::vector<sf::Vector2i>& highlightSquares) const;
    const Piece* pieceAt(sf::Vector2i position) const;
    static bool isInside(sf::Vector2i position);
    std::vector<Move> getMovesForPiece(sf::Vector2i from, bool capturesOnly) const;
    std::vector<Move> getAllMoves(PieceColor color, bool capturesOnly) const;
    bool applyMove(const Move& move);
    bool hasCaptureMoves(PieceColor color) const;
    bool playerHasMoves(PieceColor color) const;
    int countPieces(PieceColor color) const;

private:
    using Grid = std::array<std::array<std::optional<Piece>, SIZE>, SIZE>;

    Grid m_grid{};

    static bool isDarkSquare(int row, int col);
    std::vector<sf::Vector2i> moveDirectionsForPiece(const Piece& piece) const;
    void promoteIfNeeded(sf::Vector2i position);
};
