#include "pieces/knight.hpp"

Knight::Knight(PlayerColor color, Position pos) : Piece(color, pos, PieceType::KNIGHT, 3) {}

char Knight::toChar()
{
    if (color == PlayerColor::WHITE) {
        return 'N';
    } else {
        return 'n';
    }
}

std::vector<PossibleMove> Knight::getPossibleMoves(std::array<std::array<std::unique_ptr<Piece>, 8>, 8>& board)
{
    std::vector<PossibleMove> moves;
    for(auto& [dx, dy]: KNIGHT_OFFSETS) {
        Position possible_pos(position.x + dx, position.y + dy);
        if(isOnBoard(possible_pos)) {
            Piece* p = board[possible_pos.y][possible_pos.x].get();
            if (!p || (p->color != color)) {
                moves.emplace_back(possible_pos, MoveType::NORMAL);
            }
        }
    }
    
    return moves;
}