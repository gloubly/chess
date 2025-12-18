#include "pieces/queen.hpp"

Queen::Queen(PlayerColor color, Position pos) : Piece(color, pos, PieceType::QUEEN, 9) {}

char Queen::toChar()
{
    if (color == PlayerColor::WHITE) {
        return 'Q';
    } else {
        return 'q';
    }
}

std::vector<PossibleMove> Queen::getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board)
{
    std::vector<PossibleMove> moves;
    for(int dx=-1; dx<=1; ++dx) {
        for(int dy=-1; dy<=1; ++dy) {
            if(dx==0 && dy==0) {
                continue;
            }
            Position possible_pos(position.x + dx, position.y + dy);
            while (isOnBoard(possible_pos)) {
                Piece* p = board[possible_pos.y][possible_pos.x].get();
                if (!p || p->color != color) {
                    moves.emplace_back(possible_pos, MoveType::NORMAL);
                }

                if (p) {
                    // stop at first opp encounter
                    break;
                }

                possible_pos.x += dx;
                possible_pos.y += dy;
            }
        }
    }
    return moves;
}