#include "pieces/pawn.hpp"

Pawn::Pawn(PlayerColor color, Position pos) : Piece(color, pos, PieceType::PAWN, 1)
{
    if (color == PlayerColor::WHITE) {
        direction = -1;
    } else {
        direction = 1;
    }
    has_moved = !((color == PlayerColor::WHITE && position.y == 6) || (color == PlayerColor::BLACK && position.y == 1));
    if (pos.y == 0 || pos.y == 7) {
        throw std::runtime_error("invalid pawn position: " + pos.toString());
    }
}

char Pawn::toChar()
{
    if (color == PlayerColor::WHITE) {
        return 'P';
    } else {
        return 'p';
    }
}

std::string Pawn::getState() {
    std::string s(1, toChar());
    if (!has_moved) {
        s += 'i';
    }
    else if (en_passant) {
        s += 'e';
    }
    return s;
}

std::vector<PossibleMove> Pawn::getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board)
{
    std::vector<PossibleMove> moves;
    MoveType moveType;
    Position pos;
    if(position.y>0 && position.y<7) { // pawns can't be on the row behind where they started and on the last row (=> promoted)
        if(!board[position.y+direction][position.x]) {
            if ((position.y == 1 && color == PlayerColor::WHITE) || (position.y == 6 && color == PlayerColor::BLACK)) {
                moveType = MoveType::PROMOTE;
            } else {
                moveType = MoveType::NORMAL;
            }
            moves.push_back(PossibleMove(position.x, position.y+direction, moveType));
            
            if(!has_moved && !board[position.y+2*direction][position.x]) {
                if ((position.y != 1 && color == PlayerColor::BLACK) || (position.y != 6 && color == PlayerColor::WHITE)) {
                    throw std::runtime_error("position error: " + position.toString());
                }
                // 2 squares
                moves.push_back(PossibleMove(position.x, position.y+2*direction, MoveType::NORMAL));
            }
        }

        // taking left and right
        for (const int& dx : { -1, 1 }) {
            pos = Position(position.x + dx, position.y + direction);
            if(!isOnBoard(pos)) continue;
            Piece* p = board[pos.y][pos.x].get();
            if (p && p->color != color) {
                if ((position.y == 1 && color == PlayerColor::WHITE) || (position.y == 6 && color == PlayerColor::BLACK)) {
                    moveType = MoveType::PROMOTE;
                }
                else {
                    moveType = MoveType::NORMAL;
                }
                moves.push_back(PossibleMove(pos, moveType));
            }
        }

        // en passant
        if ((position.y == 3 && color == PlayerColor::WHITE) || (position.y == 4 && color == PlayerColor::BLACK)) {
            for (const int& dx : { -1, 1 }) {
                pos = Position(position.x + dx, position.y);
                if (!isOnBoard(pos)) continue;
                Piece* piece = board[pos.y][pos.x].get();
                if (piece && piece->pieceType == PieceType::PAWN && piece->color != color) {
                    Pawn* pawn = static_cast<Pawn*>(piece);
                    if (pawn->en_passant) {
                        moves.push_back(PossibleMove(position.x + dx, position.y + direction, MoveType::EN_PASSANT));
                    }
                }
            }
        }
    }
    return moves;
}

void Pawn::updatePosition(Position pos) {
    position = pos;
    has_moved = true;
}