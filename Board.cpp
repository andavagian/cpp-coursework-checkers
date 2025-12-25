#include "Board.h"

#include <algorithm>

namespace
{
constexpr float SELECTION_OUTLINE = 4.0f;
}

Board::Board()
{
    reset();
}

void Board::reset()
{
    for (auto& row : m_grid)
    {
        for (auto& cell : row)
        {
            cell.reset();
        }
    }

    for (int row = 0; row < SIZE; ++row)
    {
        for (int col = 0; col < SIZE; ++col)
        {
            if (!isDarkSquare(row, col))
            {
                continue;
            }

            if (row < 3)
            {
                m_grid[row][col].emplace(PieceColor::Second);
            }
            else if (row > 4)
            {
                m_grid[row][col].emplace(PieceColor::First);
            }
        }
    }
}

void Board::draw(sf::RenderWindow& window,
                 float cellSize,
                 const std::optional<sf::Vector2i>& selected,
                 const std::vector<sf::Vector2i>& highlightSquares) const
{
    const sf::Color lightSquare(245, 230, 200);
    const sf::Color darkSquare(101, 67, 33);

    sf::RectangleShape square(sf::Vector2f{cellSize, cellSize});
    for (int row = 0; row < SIZE; ++row)
    {
        for (int col = 0; col < SIZE; ++col)
        {
            square.setPosition({static_cast<float>(col) * cellSize,
                                static_cast<float>(row) * cellSize});
            square.setFillColor(isDarkSquare(row, col) ? darkSquare : lightSquare);

            if (selected && selected->x == row && selected->y == col)
            {
                square.setOutlineColor(sf::Color(255, 215, 0, 200));
                square.setOutlineThickness(SELECTION_OUTLINE);
            }
            else
            {
                square.setOutlineThickness(0.f);
            }

            window.draw(square);
        }
    }

    sf::CircleShape highlight(cellSize * 0.2f);
    highlight.setFillColor(sf::Color(255, 215, 0, 120));
    for (const auto& sq : highlightSquares)
    {
        highlight.setPosition({(static_cast<float>(sq.y) + 0.5f) * cellSize - highlight.getRadius(),
                               (static_cast<float>(sq.x) + 0.5f) * cellSize - highlight.getRadius()});
        window.draw(highlight);
    }

    for (int row = 0; row < SIZE; ++row)
    {
        for (int col = 0; col < SIZE; ++col)
        {
            const auto& cell = m_grid[row][col];
            if (!cell)
            {
                continue;
            }

            sf::CircleShape pieceShape(cellSize * 0.38f);
            const sf::Color fill = cell->getColor() == PieceColor::First ? sf::Color(220, 200, 170)
                                                                          : sf::Color(120, 70, 50);
            pieceShape.setFillColor(fill);
            pieceShape.setOutlineColor(sf::Color(60, 40, 25));
            pieceShape.setOutlineThickness(cell->isKing() ? 4.f : 2.5f);
            pieceShape.setPosition({(static_cast<float>(col) + 0.5f) * cellSize - pieceShape.getRadius(),
                                    (static_cast<float>(row) + 0.5f) * cellSize - pieceShape.getRadius()});
            window.draw(pieceShape);

            if (cell->isKing())
            {
                sf::CircleShape inner(pieceShape.getRadius() * 0.5f);
                inner.setFillColor(cell->getColor() == PieceColor::First ? sf::Color(240, 220, 190) : sf::Color(140, 90, 70));
                inner.setOutlineColor(sf::Color(80, 50, 30));
                inner.setOutlineThickness(1.5f);
                inner.setPosition({pieceShape.getPosition().x + pieceShape.getRadius() - inner.getRadius(),
                                   pieceShape.getPosition().y + pieceShape.getRadius() - inner.getRadius()});
                window.draw(inner);
            }
        }
    }
}

const Piece* Board::pieceAt(sf::Vector2i position) const
{
    if (!isInside(position))
    {
        return nullptr;
    }
    const auto& entry = m_grid[position.x][position.y];
    return entry ? &(*entry) : nullptr;
}

bool Board::isInside(sf::Vector2i position)
{
    return position.x >= 0 && position.x < SIZE && position.y >= 0 && position.y < SIZE;
}

std::vector<Board::Move> Board::getMovesForPiece(sf::Vector2i from, bool capturesOnly) const
{
    std::vector<Move> moves;
    if (!isInside(from))
    {
        return moves;
    }

    const auto& pieceSlot = m_grid[from.x][from.y];
    if (!pieceSlot)
    {
        return moves;
    }

    std::vector<Move> capturingMoves;
    const auto directions = moveDirectionsForPiece(*pieceSlot);

    for (const auto& dir : directions)
    {
        sf::Vector2i current = from + dir;
        bool foundEnemy = false;
        sf::Vector2i enemyPos{-1, -1};

        while (isInside(current))
        {
            const auto& cell = m_grid[current.x][current.y];

            if (!cell)
            {
                if (!capturesOnly && !foundEnemy)
                {
                    Move move;
                    move.from = from;
                    move.to = current;
                    moves.push_back(move);
                }
                else if (foundEnemy)
                {
                    Move capture;
                    capture.from = from;
                    capture.to = current;
                    capture.isCapture = true;
                    capture.captured = enemyPos;
                    capturingMoves.push_back(capture);
                }

                if (!pieceSlot->isKing())
                {
                    break;
                }
                current += dir;
                continue;
            }

            if (cell->getColor() == pieceSlot->getColor())
            {
                break;
            }

            if (foundEnemy)
            {
                break;
            }
            foundEnemy = true;
            enemyPos = current;
            current += dir;

            if (!pieceSlot->isKing())
            {
                if (isInside(current) && !m_grid[current.x][current.y])
                {
                    Move capture;
                    capture.from = from;
                    capture.to = current;
                    capture.isCapture = true;
                    capture.captured = enemyPos;
                    capturingMoves.push_back(capture);
                }
                break;
            }
        }
    }

    if (!capturingMoves.empty())
    {
        return capturingMoves;
    }

    return moves;
}

std::vector<Board::Move> Board::getAllMoves(PieceColor color, bool capturesOnly) const
{
    std::vector<Move> result;
    for (int row = 0; row < SIZE; ++row)
    {
        for (int col = 0; col < SIZE; ++col)
        {
            const auto& cell = m_grid[row][col];
            if (!cell || cell->getColor() != color)
            {
                continue;
            }

            auto moves = getMovesForPiece({row, col}, capturesOnly);
            result.insert(result.end(), moves.begin(), moves.end());
        }
    }
    return result;
}

bool Board::applyMove(const Move& move)
{
    if (!isInside(move.from) || !isInside(move.to))
    {
        return false;
    }

    auto& fromCell = m_grid[move.from.x][move.from.y];
    if (!fromCell || m_grid[move.to.x][move.to.y])
    {
        return false;
    }

    m_grid[move.to.x][move.to.y] = fromCell;
    fromCell.reset();

    if (move.isCapture && isInside(move.captured))
    {
        m_grid[move.captured.x][move.captured.y].reset();
    }

    promoteIfNeeded(move.to);
    return true;
}

bool Board::hasCaptureMoves(PieceColor color) const
{
    for (int row = 0; row < SIZE; ++row)
    {
        for (int col = 0; col < SIZE; ++col)
        {
            const auto& cell = m_grid[row][col];
            if (!cell || cell->getColor() != color)
            {
                continue;
            }

            auto moves = getMovesForPiece({row, col}, true);
            if (!moves.empty())
            {
                return true;
            }
        }
    }
    return false;
}

bool Board::playerHasMoves(PieceColor color) const
{
    const bool mustCapture = hasCaptureMoves(color);
    auto moves = getAllMoves(color, mustCapture);
    return !moves.empty();
}

int Board::countPieces(PieceColor color) const
{
    int total = 0;
    for (const auto& row : m_grid)
    {
        for (const auto& cell : row)
        {
            if (cell && cell->getColor() == color)
            {
                ++total;
            }
        }
    }
    return total;
}

bool Board::isDarkSquare(int row, int col)
{
    return (row + col) % 2 == 1;
}

std::vector<sf::Vector2i> Board::moveDirectionsForPiece(const Piece& piece) const
{
    std::vector<sf::Vector2i> directions;
    const int forward = piece.getColor() == PieceColor::First ? -1 : 1;
    if (piece.isKing())
    {
        directions.push_back({1, 1});
        directions.push_back({1, -1});
        directions.push_back({-1, 1});
        directions.push_back({-1, -1});
    }
    else
    {
        directions.push_back({forward, -1});
        directions.push_back({forward, 1});
    }
    return directions;
}

void Board::promoteIfNeeded(sf::Vector2i position)
{
    auto& cell = m_grid[position.x][position.y];
    if (!cell)
    {
        return;
    }

    if (cell->getColor() == PieceColor::First && position.x == 0)
    {
        cell->promoteToKing();
    }
    else if (cell->getColor() == PieceColor::Second && position.x == SIZE - 1)
    {
        cell->promoteToKing();
    }
}
