#include "board.hpp"

Alignment getAlignment(Position p1, Position p2);

Board::Board() : Board(
    "rnbqkbnr"
    "pppppppp"
    "        "
    "        "
    "        "
    "        "
    "PPPPPPPP"
    "RNBQKBNR"
) {}

Board::Board(const std::string& board_str) {
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            board_array[i][j] = nullptr;
        }
    }

    int i(0);
    for (const char& c : board_str) {
        switch (c) {
            case 'r':
                board_array[i / 8][i % 8] = std::make_unique<Rook>(PlayerColor::BLACK, Position{ i % 8, i / 8 });
                break;
            case 'b':
                board_array[i / 8][i % 8] = std::make_unique<Bishop>(PlayerColor::BLACK, Position{ i % 8, i / 8 });
                break;
            case 'n':
                board_array[i / 8][i % 8] = std::make_unique<Knight>(PlayerColor::BLACK, Position{ i % 8, i / 8 });
                break;
            case 'q':
                board_array[i / 8][i % 8] = std::make_unique<Queen>(PlayerColor::BLACK, Position{ i % 8, i / 8 });
                break;
            case 'k':
                board_array[i / 8][i % 8] = std::make_unique<King>(PlayerColor::BLACK, Position{ i % 8, i / 8 });
                break;
            case 'p':
                board_array[i / 8][i % 8] = std::make_unique<Pawn>(PlayerColor::BLACK, Position{ i % 8, i / 8 });
                break;
            case 'P':
                board_array[i / 8][i % 8] = std::make_unique<Pawn>(PlayerColor::WHITE, Position{ i % 8, i / 8 });
                break;
            case 'R':
                board_array[i / 8][i % 8] = std::make_unique<Rook>(PlayerColor::WHITE, Position{ i % 8, i / 8 });
                break;
            case 'N':
                board_array[i / 8][i % 8] = std::make_unique<Knight>(PlayerColor::WHITE, Position{ i % 8, i / 8 });
                break;
            case 'B':
                board_array[i / 8][i % 8] = std::make_unique<Bishop>(PlayerColor::WHITE, Position{ i % 8, i / 8 });
                break;
            case 'Q':
                board_array[i / 8][i % 8] = std::make_unique<Queen>(PlayerColor::WHITE, Position{ i % 8, i / 8 });
                break;
            case 'K':
                board_array[i / 8][i % 8] = std::make_unique<King>(PlayerColor::WHITE, Position{ i % 8, i / 8 });
                break;
            default:
                break;
        }
        ++i;
    }

    // kings check and position
    for (size_t y = 0; y < 8; ++y) {
        for (size_t x = 0; x < 8; ++x) {
            Piece* p = board_array[y][x].get();
            if (p && p->pieceType == PieceType::KING) {
                if (king_positions[toIndex(p->color)].x != -1) {
                    throw std::runtime_error("multiple same color king found");
                }
                king_positions[toIndex(p->color)] = p->position;
                auto king = static_cast<King*>(p);
                king->checked = king->isCheckedAt(board_array, king->position);
            }
        }
    }

    if (king_positions[toIndex(PlayerColor::WHITE)].x == -1 || king_positions[toIndex(PlayerColor::WHITE)].y == -1) {
        throw std::exception("missing a king");
    }
}

void Board::movePiece(Position current_pos, const PossibleMove& move) {
    // check that rook hasn't moved when rocking
    switch (move.moveType) {
        case MoveType::CASTLE:
            // move rook
            if (move.position.x == 6) {
                // kingside
                board_array[current_pos.y][5] = std::move(board_array[current_pos.y][7]);
                at(5, current_pos.y)->updatePosition({ 5, current_pos.y });
            }
            else {
                // queenside
                board_array[current_pos.y][3] = std::move(board_array[current_pos.y][0]);
                at(3, current_pos.y)->updatePosition({ 3, current_pos.y });
            }
            break;
        case MoveType::EN_PASSANT:
            // clear opp pawn
            board_array[current_pos.y][move.position.x] = nullptr;
            break;
        default:
            break;
    }

    if (at(move.position) || at(current_pos)->pieceType==PieceType::PAWN || move.moveType == MoveType::PROMOTE) {
        // reset when take or pawn movement
        move_count = 0;
    }
    else {
        move_count++;
    }
    board_array[move.position.y][move.position.x] = std::move(board_array[current_pos.y][current_pos.x]);
    board_array[move.position.y][move.position.x]->updatePosition(move.position);

    Piece* piece = at(move.position.x, move.position.y);
    if (piece->pieceType == PieceType::KING) {
        king_positions[toIndex(piece->color)] = move.position;
    }
    else if (piece->pieceType == PieceType::PAWN && std::abs(current_pos.y - move.position.y) == 2) {
        auto pawn = static_cast<Pawn*>(at(move.position));
        pawn->en_passant = true;
    }
}

void Board::promote(Position current_pos, PieceType promotion) {
    assert(at(current_pos)->pieceType == PieceType::PAWN);
    Piece* p = at(current_pos);

    switch (promotion) {
        case PieceType::BISHOP:
            board_array[current_pos.y][current_pos.x] = std::make_unique<Bishop>(p->color, p->position);
            break;
        case PieceType::KNIGHT:
            board_array[current_pos.y][current_pos.x] = std::make_unique<Knight>(p->color, p->position);
            break;
        case PieceType::QUEEN:
            board_array[current_pos.y][current_pos.x] = std::make_unique<Queen>(p->color, p->position);
            break;
        case PieceType::ROOK:
            board_array[current_pos.y][current_pos.x] = std::make_unique<Rook>(p->color, p->position);
            break;
        default:
            throw std::runtime_error("wrong piece type");
            break;
    }
    move_count = 0; // pawn movement
}

void Board::disableEnPassant(PlayerColor player_color) {
    size_t y = (player_color == PlayerColor::BLACK) ? 3 : 4;
    for (int x = 0; x < 8; ++x) {
        Piece* piece = at(x, y);
        if (piece && piece->pieceType == PieceType::PAWN) {
            static_cast<Pawn*>(piece)->en_passant = false;
        }
    }
}

std::vector<PossibleMove> Board::filterLegalMoves(Position pos) {
    std::vector<PossibleMove> possible_moves;

    std::vector<Position> check_pieces_positions;

    Piece* piece = at(pos);
    auto king = static_cast<King*>(at(king_positions[toIndex(piece->color)]));
    
    possible_moves = piece->getPossibleMoves(board_array);
    if (king->checked && piece->pieceType != PieceType::KING) {
        check_pieces_positions = getCheckPieces(piece->color);
        assert(!check_pieces_positions.empty());
        if (check_pieces_positions.size() == 1) {
            Position check_piece_pos = check_pieces_positions[0];
            std::vector<Position> allowed_positions = { check_piece_pos };
            if (at(check_piece_pos)->pieceType != PieceType::KNIGHT) {
                // from piece to king
                int dir_x = sign(king->position.x - check_piece_pos.x);
                int dir_y = sign(king->position.y - check_piece_pos.y);
                Position pos = { check_piece_pos.x + dir_x, check_piece_pos.y + dir_y };
                while (pos != king->position) {
                    allowed_positions.push_back(pos);
                    pos.x += dir_x;
                    pos.y += dir_y;
                }
                std::erase_if(possible_moves, [&allowed_positions](const PossibleMove& pm) {
                    return std::find(allowed_positions.begin(), allowed_positions.end(), pm.position) == allowed_positions.end();
                    });
            }
        }
        else {
            // multiple checks, can only move the king
            return std::vector<PossibleMove>();
        }
    }

    if (piece->pieceType != PieceType::KING) {
        std::erase_if(possible_moves, [this, &piece](const PossibleMove& pm) { return isDiscovery(piece, pm.position); });
    }
    return possible_moves;
}

std::vector<Position> Board::getCheckPieces(PlayerColor color) {
    std::vector<Position> check_piece_positions;

    Position king_pos = king_positions[toIndex(color)];

    // check for knight
    for (auto& [dx, dy] : KNIGHT_OFFSETS) {
        Position piece_pos = { king_pos.x + dx, king_pos.y + dy };
        if (!isOnBoard(piece_pos)) continue; // not defined
        Piece* piece = at(piece_pos);
        if (piece && piece->pieceType == PieceType::KNIGHT && piece->color != color) {
            check_piece_positions.push_back(piece_pos);
        }
    }

    // check for opp guarding
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            if (dx == 0 && dy == 0) continue;  // actual possible position
            Position piece_pos = { king_pos.x + dx, king_pos.y + dy };

            while (isOnBoard(piece_pos)) {
                Piece* piece = at(piece_pos);
                if (piece) {
                    if (piece->color != color) {
                        if (piece->pieceType == PieceType::PAWN) {
                            if (
                                ((color == PlayerColor::WHITE && piece_pos.y == king_pos.y - 1) || (color == PlayerColor::BLACK && piece_pos.y == king_pos.y + 1))
                                && (std::abs(piece_pos.x - king_pos.x) == 1)
                                ) {
                                check_piece_positions.push_back(piece_pos);
                            }
                        }
                        else if (piece->pieceType == PieceType::QUEEN) {
                            check_piece_positions.push_back(piece_pos);
                        }
                        else if (piece->pieceType == PieceType::ROOK && (dx == 0 || dy == 0)) {
                            check_piece_positions.push_back(piece_pos);
                        }
                        else if (piece->pieceType == PieceType::BISHOP && (dx != 0 && dy != 0)) {
                            check_piece_positions.push_back(piece_pos);
                        }
                        else if (piece->pieceType == PieceType::KING && std::max(std::abs(piece_pos.x - king_pos.x), std::abs(piece_pos.y - king_pos.y)) == 1) {
                            check_piece_positions.push_back(piece_pos);
                        }
                    }
                    break; // piece encountered
                }
                piece_pos.x += dx;
                piece_pos.y += dy;
            }
        }
    }
    return check_piece_positions;
}

std::string Board::getState(PlayerColor player_to_move) {
    std::string board_state = std::to_string(toIndex(player_to_move));
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece* p = at(x, y);
            if (p) {
                board_state += p->getState();
            }
            else {
                board_state += " ";
            }
        }
    }
    return board_state;
}

bool Board::isDiscovery(Piece* piece, Position possible_pos) {
    bool opp = false;

    Position opp_pos;
    Position king_pos = king_positions[toIndex(piece->color)];

    Alignment current_align = getAlignment(piece->position, king_pos);
    Alignment next_align = getAlignment(possible_pos, king_pos);
    
    int dir_x = sign(king_pos.x - piece->position.x);
    int dir_y = sign(king_pos.y - piece->position.y);

    if (current_align==Alignment::NONE) {
        return false;
    }

    // find opponent
    Position pos = { piece->position.x - dir_x, piece->position.y - dir_y };
    while (isOnBoard(pos)) {
        if (pos != piece->position) {
            Piece* other_piece = at(pos);
            if (other_piece) {
                if (other_piece->color != piece->color) {
                    if (
                        other_piece->pieceType == PieceType::QUEEN ||
                        (other_piece->pieceType == PieceType::ROOK && current_align == Alignment::STRAIGHT) ||
                        (other_piece->pieceType == PieceType::BISHOP && current_align == Alignment::DIAGONAL)
                        ) {
                        opp_pos = pos;
                        opp = true;
                    }
                }
                break;
            }
        }
        pos.x -= dir_x;
        pos.y -= dir_y;
    }
    if (opp) {
        if (dir_x == sign(king_pos.x - possible_pos.x) && dir_y == sign(king_pos.y - possible_pos.y) && current_align == next_align) {
            // will keep the same alignment
            return false;
        }
        else {
            // need to search for a "shield" piece
            bool shield = false;
            pos = { piece->position.x + dir_x, piece->position.y + dir_y };
            while (pos != king_pos) {
                Piece* p = at(pos);
                if (p) {
                    shield = true;
                    break;
                }
                pos.x += dir_x;
                pos.y += dir_y;
            }
            return !shield;
        }        
    }
    else {
        return false;
    }
}

bool Board::is50Moves() {
    return move_count >= 50;
}

bool Board::isStalemate(PlayerColor player_to_move) {
    // stalemate: the player to move is not in check and has no legal move
    auto king = static_cast<King*>(at(king_positions[toIndex(player_to_move)]));
    if (king->checked || !king->getPossibleMoves(board_array).empty()) {
        return false;
    }

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece* piece = at(x, y);
            if (piece && piece->color == player_to_move && piece->pieceType!=PieceType::KING && !filterLegalMoves({x, y}).empty()) {
                return false;
            }
        }
    }
    return true;
}

bool Board::isCheckmate(PlayerColor player_to_move) {
    Position king_pos = king_positions[toIndex(player_to_move)];
    auto king = static_cast<King*>(at(king_pos));
    if (!king->checked || !filterLegalMoves(king_pos).empty()) {
        return false;
    }
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece* p = at(x, y);
            if (p && p->color == player_to_move && !filterLegalMoves(p->position).empty()) {
                return false;
            }
        }
    }
    return true;
}

bool Board::isDeadPosition() {
    // k vs k
    // k vs k & b
    // k vs k & n
    std::array<int, 2> bishop_count[2] = { {0, 0}, {0, 0} };
    int knight_count[2] = { 0 };
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece* p = at(x, y);
            if (p) {
                if (p->pieceType == PieceType::BISHOP) {
                    int tile_type = (p->position.x + p->position.y) % 2;
                    bishop_count[toIndex(p->color)][tile_type] += 1;
                }
                else if (p->pieceType == PieceType::KNIGHT) {
                    knight_count[toIndex(p->color)] += 1;
                }
                else if (p->pieceType!=PieceType::KING && (p->pieceType!=PieceType::PAWN || !p->getPossibleMoves(board_array).empty())) {
                    // is pawn and can move or not a king
                    return false;
                }
            }
        }
    }
    // need bishops on both black and white tiles
    int light_bishops = bishop_count[toIndex(PlayerColor::WHITE)][0] + bishop_count[toIndex(PlayerColor::BLACK)][0];
    int dark_bishops = bishop_count[toIndex(PlayerColor::WHITE)][1] + bishop_count[toIndex(PlayerColor::BLACK)][1];


    bool unsifficient_knights = (knight_count[toIndex(PlayerColor::WHITE)] + knight_count[toIndex(PlayerColor::BLACK)]) <= 1;

    return unsifficient_knights && (light_bishops == 0 || dark_bishops == 0);
}

void Board::updateChecks() {
    for (const auto& pos: king_positions) {
        auto king = static_cast<King*>(at(pos));
        king->checked = king->isCheckedAt(board_array, pos);
    }
}

std::tuple<int, int> Board::getPlayerScores() {
    int white_score = 0;
    int black_score = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; ++x) {
            Piece* p = at(x, y);
            if (p) {
                if (p->color == PlayerColor::WHITE) {
                    white_score += p->value;
                }
                else black_score += p->value;
            }
        }
    }
    return { black_score, white_score };
}

std::string Board::toString() {
    std::string s;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            Piece* p = at(x, y);
            if (p) {
                s += p->toChar();
            }
            else {
                s+= ' ';
            }
        }
        s += '\n';
    }
    return s;
}

Piece* Board::at(int x, int y) const {
	return board_array[y][x].get();
}

Piece* Board::at(Position pos) const {
    return this->board_array[pos.y][pos.x].get();
}


Alignment getAlignment(Position p1, Position p2) {
    auto align = Alignment::NONE;
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;
    if (dx == 0 || dy == 0) {
        align = Alignment::STRAIGHT;
    }
    else if (std::abs(dx) == std::abs(dy)) {
        align = Alignment::DIAGONAL;
    }
    return align;
}