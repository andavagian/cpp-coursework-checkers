#include "Piece.h"

Piece::Piece(PieceColor color, bool king)
    : m_color(color)
    , m_isKing(king)
{
}

PieceColor Piece::getColor() const
{
    return m_color;
}

bool Piece::isKing() const
{
    return m_isKing;
}

void Piece::promoteToKing()
{
    m_isKing = true;
}

void Piece::setKing(bool kingValue)
{
    m_isKing = kingValue;
}

std::string Piece::colorName() const
{
    return m_color == PieceColor::First ? "First" : "Second";
}
