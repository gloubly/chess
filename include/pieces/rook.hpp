#pragma once
#include "piece.hpp"


class Rook : public Piece
{
public:
    Rook(PlayerColor color, Position pos);

    ~Rook() = default;

    char toChar();
    
    std::string getState() override; // R/r (+ i if initial position)
    
    std::vector<PossibleMove> getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);

    void updatePosition(Position pos) override;

    bool has_moved;
};