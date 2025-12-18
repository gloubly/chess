#pragma once
#include "piece.hpp"


class Bishop : public Piece
{
public:
    Bishop(PlayerColor color, Position pos);

    ~Bishop() = default;

    char toChar();

    std::vector<PossibleMove> getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);
};