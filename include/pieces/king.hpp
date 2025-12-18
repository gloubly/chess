#pragma once
#include "piece.hpp"
#include "pieces/knight.hpp"
#include "pieces/rook.hpp"


class King : public Piece
{
public:
    King(PlayerColor color, Position pos);

    ~King() = default;

    char toChar();

    std::string getState() override; // K/k (+ i if initial position)

    std::vector<PossibleMove> getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board);

    bool isCheckedAt(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board, Position pos);

    bool canMoveAt(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board, Position possible_pos);

    void updatePosition(Position pos) override;

    bool has_moved;

    bool checked;
};