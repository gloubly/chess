#include "pieces/king.hpp"

King::King(PlayerColor color, Position pos) : Piece(color, pos, PieceType::KING, 0) {
    has_moved = !((color== PlayerColor::WHITE && position.x==4 && position.y==7) || (color== PlayerColor::BLACK && position.x==4 && position.y==0));
    checked = false;
}

char King::toChar()
{
    if(color== PlayerColor::WHITE) {
        return 'K';
    } else {
        return 'k';
    }
}

std::string King::getState() {
    std::string s(1, toChar());
    if (!has_moved) {
        s += 'i';
    }
    return s;
}

std::vector<PossibleMove> King::getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board)
{
    std::vector<PossibleMove> moves;
    for(int dx=-1; dx<=1; ++dx) {
        for(int dy=-1; dy<=1; ++dy) {
            if(dx==0 && dy==0) {
                continue;
            }
            Position possible_pos(position.x + dx, position.y + dy);
            if(isOnBoard(possible_pos) && King::canMoveAt(board, possible_pos)) {
                moves.push_back(PossibleMove(possible_pos, MoveType::NORMAL));
            }
        }
    }
    // castle
    if (!has_moved && !checked) {
        if (!(position.x == 4 && ((position.y == 0 && color == PlayerColor::BLACK) || (position.y == 7 && color == PlayerColor::WHITE)))) {
            throw std::runtime_error("king position error: " + position.toString());
        }

        // king size
        Piece* piece = board[position.y][7].get();
        if (piece && piece->pieceType == PieceType::ROOK) {
            auto rook = static_cast<Rook*>(piece);
            if (!rook->has_moved && rook->color == color && !board[position.y][5] && !board[position.y][6]) {
                moves.push_back(PossibleMove(6, position.y, MoveType::CASTLE));
            }
        }
        piece = board[position.y][0].get();
        if (piece && piece->pieceType == PieceType::ROOK) {
            auto rook = static_cast<Rook*>(piece);
            if (
                !rook->has_moved && rook->color == color &&
                !board[position.y][1] && !board[position.y][2] && !board[position.y][3]
                ) {
                moves.push_back(PossibleMove(2, position.y, MoveType::CASTLE));
            }
        }
    }
    return moves;
}

bool King::isCheckedAt(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board, Position pos)
{
    std::vector<Position> check_piece_positions;

    // check for knight
    for (auto& [dx, dy] : KNIGHT_OFFSETS) {
        Position piece_pos = { pos.x + dx, pos.y + dy };
        if (!isOnBoard(piece_pos)) continue; // not defined
        Piece* piece = board[piece_pos.y][piece_pos.x].get();
        if (piece && piece->pieceType == PieceType::KNIGHT && piece->color != color) {
            check_piece_positions.push_back(piece_pos);
            return true;
        }
    }

    // check for opp guarding
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) { // actual possible position
                continue;
            }
            Position piece_pos = { pos.x + dx, pos.y + dy };
            while (isOnBoard(piece_pos)) {
                Piece* piece = board[piece_pos.y][piece_pos.x].get();
                if (piece && piece_pos != position) {
                    if (piece->color != color) {
                        if (piece->pieceType == PieceType::PAWN) {
                            if (
                                ((color == PlayerColor::WHITE && piece_pos.y == pos.y - 1) || (color == PlayerColor::BLACK && piece_pos.y == pos.y + 1))
                                && (std::abs(piece_pos.x - pos.x) == 1)
                                ) {
                                return true;
                            }
                        }
                        else if (piece->pieceType == PieceType::QUEEN) {
                            check_piece_positions.push_back(piece_pos);
                            return true;
                        }
                        else if (piece->pieceType == PieceType::ROOK && (dx == 0 || dy == 0)) {
                            check_piece_positions.push_back(piece_pos);
                            return true;
                        }
                        else if (piece->pieceType == PieceType::BISHOP && (dx != 0 && dy != 0)) {
                            check_piece_positions.push_back(piece_pos);
                            return true;
                        }
                        else if (piece->pieceType == PieceType::KING && std::max(std::abs(piece_pos.x - pos.x), std::abs(piece_pos.y - pos.y)) == 1) {
                            // is king and at 1 tile from current
                            check_piece_positions.push_back(piece_pos);
                            return true;
                        }
                    }
                    break; // piece encountered
                }
                piece_pos.x += dx;
                piece_pos.y += dy;
            }
        }
    }
    return false;
}

bool King::canMoveAt(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board, Position possible_pos)
{
    Piece* piece = board[possible_pos.y][possible_pos.x].get();
    // is friend piece ?
    if (piece && piece->color == color) {
        return false;
    }
    return !isCheckedAt(board, possible_pos);
}

void King::updatePosition(Position pos) {
    position = pos;
    has_moved = true;
}