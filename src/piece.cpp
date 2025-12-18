#include "piece.hpp"

Piece::Piece(PlayerColor color, Position pos, PieceType pieceType, int value): color(color), position(pos), pieceType(pieceType), value(value) {}

std::string Piece::getState() {
	return std::string(1, toChar());
}

void Piece::updatePosition(Position pos) {
	position = pos;
}