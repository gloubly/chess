#pragma once
#include "piece.hpp"

const std::pair<int, int> KNIGHT_OFFSETS[] {
    // {dx, dy}
    {-1, -2},
    {1, -2},
    {2, -1},
    {2, 1},
    {1, 2},
    {-1, 2},
    {-2, 1},
    {-2, -1}
};

class Knight : public Piece
{
public:
    Knight(PlayerColor color, Position pos);

    ~Knight() = default;

    char toChar();

    std::vector<PossibleMove> getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);
};