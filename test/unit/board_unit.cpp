#include <gtest/gtest.h>
#include "board.hpp"

TEST(BoardTest, discoveryTest) {
	Board board = Board(
		"k       "
		"        "
		"        "
		"        "
		"        "
		"        "
		"r P K   "
		"       r"
	);
	EXPECT_TRUE(board.filterLegalMoves({ 2, 6 }).empty());

	board = Board(
		"        "
		"        "
		"        "
		"        "
		"        "
		"p Q     "
		" p      "
		"k K     "
	);
	EXPECT_TRUE(board.filterLegalMoves({ 1, 6 }).empty());
	EXPECT_FALSE(board.filterLegalMoves({ 0, 5 }).empty());
}

TEST(BoardTest, promotionTest) {
	std::string board_str = (
		"        "
		"   P    "
		"        "
		"K       "
		"        "
		"k       "
		"   p    "
		"        "
		);

	Board board = Board(board_str);
	EXPECT_EQ(1, board.filterLegalMoves({ 3, 1 }).size());
	EXPECT_EQ(1, board.filterLegalMoves({ 3, 6 }).size());
	
	PieceType types[] = {
		PieceType::QUEEN,
		PieceType::KNIGHT,
		PieceType::ROOK,
		PieceType::BISHOP
	};

	for (const PlayerColor& c : { PlayerColor::WHITE, PlayerColor::BLACK }) {
		for (const PieceType& type : types) {
			board = Board(board_str);
			Position current_pos = (c == PlayerColor::WHITE) ? Position(3, 1) : Position(3, 6);
			EXPECT_EQ(1, board.filterLegalMoves(current_pos).size());
			Position next_pos = board.filterLegalMoves(current_pos)[0].position;
			board.promote(current_pos, type);
			board.movePiece(current_pos, PossibleMove(next_pos, MoveType::PROMOTE));
			Piece* p = board.at(next_pos);
			EXPECT_NE(p, nullptr);
			EXPECT_EQ(p->pieceType, type);
		}
	}

	

}

TEST(BoardTest, checkmateTest) {
	Board board = Board(
		"        "
		"K k     "
		"        "
		"        "
		"q       "
		"        "
		"        "
		"        "
	);
	EXPECT_TRUE(board.isCheckmate(PlayerColor::WHITE));
	EXPECT_FALSE(board.isCheckmate(PlayerColor::BLACK));
	
	board = Board(
		"        "
		"K k     "
		"        "
		"        "
		"q       "
		" P      "
		"        "
		"        "
	);
	EXPECT_FALSE(board.isCheckmate(PlayerColor::WHITE));
}

TEST(BoardTest, fiftyRuleTest) {
	Board board = Board(
		"R       "
		"        "
		"P       "
		"        "
		"        "
		"        "
		"        "
		"     k K"
	);
	auto current_pos = Position(0, 0);
	auto next_pos = Position(0, 1);
	for (int i = 0; i < 50; ++i) {
		board.movePiece(current_pos, PossibleMove(next_pos, MoveType::NORMAL));
		std::swap(current_pos, next_pos);
	}
	EXPECT_TRUE(board.is50Moves());
	board.movePiece({ 0, 2 }, PossibleMove({ 0,1 }, MoveType::NORMAL));
	EXPECT_FALSE(board.is50Moves());
}

TEST(BoardTest, stalemateTest) {
	Board board = Board(
		"k       "
		"        "
		"        "
		"        "
		"        "
		"      q "
		"     r  "
		"       K"
	);
	EXPECT_TRUE(board.isStalemate(PlayerColor::WHITE));
	EXPECT_FALSE(board.isCheckmate(PlayerColor::WHITE));

	EXPECT_FALSE(board.isStalemate(PlayerColor::BLACK));

	board = Board(
		"K       "
		"        "
		"        "
		"        "
		"        "
		"      Q "
		"p    R  "
		"       k"
	);
	EXPECT_FALSE(board.isStalemate(PlayerColor::BLACK));

	board = Board(
		"        "
		"p       "
		"K       "
		"        "
		"        "
		"      Q "
		"     R  "
		"       k"
	);
	EXPECT_TRUE(board.isStalemate(PlayerColor::BLACK));

	board = Board(
		"        "
		"        "
		"        "
		"        "
		"    k n "
		"   r    "
		"r P K   "
		"r       "
	);
	EXPECT_TRUE(board.isStalemate(PlayerColor::WHITE));
}

TEST(BoardTest, deadPositionTest) {
	Board board = Board(
		"        "
		"b       "
		"kb      "
		"        "
		"        "
		"        "
		"        "
		"K       "
	);
	EXPECT_TRUE(board.isDeadPosition());

	board = Board(
		"b       "
		"b       "
		"kb      "
		"        "
		"        "
		"        "
		"        "
		"K       "
	);
	EXPECT_FALSE(board.isDeadPosition());

	board = Board(
		"bB      "
		"        "
		"k       "
		"        "
		"        "
		"        "
		"        "
		"K       "
	);
	EXPECT_FALSE(board.isDeadPosition());

	board = Board(
		"n       "
		"       "
		"k      "
		"        "
		"        "
		"        "
		"        "
		"K       "
	);
	EXPECT_TRUE(board.isDeadPosition());

	board = Board(
		"nn      "
		"       "
		"k      "
		"        "
		"        "
		"        "
		"        "
		"K       "
	);
	EXPECT_FALSE(board.isDeadPosition());

	board = Board(
		"nN      "
		"       "
		"k      "
		"        "
		"        "
		"        "
		"        "
		"K       "
	);
	EXPECT_FALSE(board.isDeadPosition());

	board = Board(
		"R      "
		"       "
		"k      "
		"        "
		"        "
		"        "
		"        "
		"K       "
	);
	EXPECT_FALSE(board.isDeadPosition());

	board = Board(
		"n      "
		"       "
		"k      "
		"        "
		"p       "
		"P       "
		"        "
		"K       "
	);
	EXPECT_TRUE(board.isDeadPosition());
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}