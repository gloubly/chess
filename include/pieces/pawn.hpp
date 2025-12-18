#pragma once
#include "piece.hpp"


class Pawn : public Piece
{
public:
    Pawn(PlayerColor color, Position pos);

    ~Pawn() = default;

    char toChar();

    std::string getState() override; // P/p (+ i if initial position or e for en passant)

    std::vector<PossibleMove> getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);

    bool en_passant = false;
    bool has_moved;
    int direction; // -1 or 1
    
    void updatePosition(Position pos) override;
};