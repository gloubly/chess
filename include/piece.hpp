#pragma once
#include <vector>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <assert.h>
#include <string>
#include <array>

enum class MoveType {NORMAL, PROMOTE, CASTLE, EN_PASSANT};

enum class PlayerColor { 
    BLACK, 
    WHITE,
    NONE // for game winner
};

enum PieceType {
    PAWN = 0, 
    BISHOP,
    KNIGHT,
    ROOK,
    QUEEN,
    KING,
    COUNT
};

enum class Promotion {NONE, QUEEN, BISHOP, ROOK, KNIGHT};

struct Position
{
    int x, y;
    Position(int x, int y): x(x), y(y) {};

    Position() : x(-1), y(-1) {};

    void clear() {
        x = -1;
        y = -1;
    }

    bool operator==(Position pos) const {
        return this->x == pos.x && this->y == pos.y;
    }

    std::string toString() const {
        return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
    }
};

struct PossibleMove
{
    Position position;
    MoveType moveType;
    PossibleMove(int x, int y, MoveType moveType): position(x,y), moveType(moveType) {};
    PossibleMove(Position pos, MoveType type) : position(pos), moveType(type) {};
};

class Piece
{
public:
    Piece(PlayerColor color, Position pos, PieceType pieceType, int value);

    Piece(const Piece& piece) = default;

    //Piece() = default;

    virtual ~Piece() = default;

    virtual std::vector<PossibleMove> getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board) = 0;

    virtual char toChar() = 0;

    virtual std::string getState();

    virtual void updatePosition(Position pos);

    Position position;
    PlayerColor color;
    PieceType pieceType;
    int value;
};

bool inline isOnBoard(Position p) {
    return p.x >= 0 && p.x < 8 && p.y >= 0 && p.y < 8;
}

PlayerColor inline otherPlayer(PlayerColor pc) {
    return (pc == PlayerColor::WHITE) ? PlayerColor::BLACK : PlayerColor::WHITE;
}

std::string inline playerToString(PlayerColor pc) {
    std::string s;
    switch (pc) {
        case PlayerColor::WHITE:
            s = "white";
            break;
        case PlayerColor::BLACK:
            s = "black";
            break;
        default:
            s = "none";
            break;
    }
    return s;
}

constexpr int toIndex(PlayerColor pc) noexcept {
    return static_cast<int>(pc);
}