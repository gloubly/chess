#pragma once

#include "pieces/pawn.hpp"
#include "pieces/bishop.hpp"
#include "pieces/knight.hpp"
#include "pieces/rook.hpp"
#include "pieces/king.hpp"
#include "pieces/queen.hpp"

#include <iostream>
#include <locale>
#include <memory>
#include <string>
#include <array>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <tuple>

enum class Alignment {
    DIAGONAL,
    STRAIGHT,
    NONE
};

const PieceType promotion_order[] = {
    PieceType::QUEEN,
    PieceType::ROOK,
    PieceType::BISHOP,
    PieceType::KNIGHT
};


class Board {
public:
    Board();

    Board(const std::string& board_str);

    void movePiece(Position current_pos, const PossibleMove& move);

    void promote(Position current_pos, PieceType promotion);

    void disableEnPassant(PlayerColor player_color);

    std::vector<PossibleMove> filterLegalMoves(Position pos);

    std::vector<Position> getCheckPieces(PlayerColor color);

    std::string getState(PlayerColor player_to_move);

    bool isDiscovery(Piece* p, Position possible_pos);

    bool is50Moves(); // 50 moves without capture or pawn movement

    bool isStalemate(PlayerColor player_to_move); // stalemate: the player to move is not in check and has no legal move

    bool isDeadPosition();

    bool isCheckmate(PlayerColor player_to_move);

    void updateChecks();

    std::tuple<int, int> getPlayerScores();

    std::string toString();

    Piece* at(int x, int y) const;
    Piece* at(Position pos) const;

private:
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board_array;

    std::vector<PossibleMove> possible_moves;

    std::vector<Position> check_pieces;

    Position king_positions[2];

    int move_count = 0;
};

int inline sign(int x) {
    return (0 < x) - (x < 0);
}