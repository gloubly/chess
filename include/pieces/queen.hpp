#pragma once
#include "piece.hpp"
#include "pawn.hpp"


class Queen : public Piece
{
public:
    Queen(PlayerColor color, Position pos);

    ~Queen() = default;

    char toChar();

    std::vector<PossibleMove> getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);
};