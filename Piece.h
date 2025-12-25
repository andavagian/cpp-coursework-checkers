#pragma once

#include <string>

enum class PieceColor
{
    First,
    Second
};

class Piece
{
public:
    explicit Piece(PieceColor color, bool king = false);
    PieceColor getColor() const;
    bool isKing() const;
    void promoteToKing();
    void setKing(bool kingValue);
    std::string colorName() const;

private:
    PieceColor m_color;
    bool m_isKing;
};
