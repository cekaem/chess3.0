#include "Board.h"
#include "MoveCalculator.h"
#include "utils/Test.h"

#include <iostream>

namespace {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

std::ostream& operator<<(std::ostream& os, const Move& move) {
  os << static_cast<char>(move.old_x + 'a') << static_cast<char>(move.old_y + '1') << "-";
  os << static_cast<char>(move.new_x + 'a') << static_cast<char>(move.new_y + '1');
  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Move>& moves) {
  for (const auto& move: moves) {
    os << move << std::endl;
  }
  return os;
}

#pragma GCC diagnostic pop

bool MovesContainMove(const std::vector<Move>& moves, const std::string& move, bool figure_captured = false) {
  const size_t old_x = move[0] - 'a';
  const size_t old_y = move[1] - '1';
  const size_t new_x = move[2] - 'a';
  const size_t new_y = move[3] - '1';
  for (const auto& move: moves) {
    if (move.old_x == old_x && move.old_y == old_y &&
        move.new_x == new_x && move.new_y == new_y &&
        move.figure_captured == figure_captured) {
      return true;
    }
  }
  return false;
}

#define VERIFY_MOVES_WITHOUT_CAPTURE(moves, old_square, new_squares) \
  VerifyMovesWithoutCapture(moves, old_square, new_squares)

void VerifyMovesWithoutCapture(
    const std::vector<Move>& moves,
    const std::string& old_square,
    const std::string& new_squares) {
  VERIFY_EQUALS(new_squares.length() % 2, 0u);
  for (size_t i = 0; i < new_squares.length(); i += 2) {
    const std::string move = old_square + new_squares.substr(i, 2);
    VERIFY_TRUE(MovesContainMove(moves, move)) << "missing move " << move;
  }
}


// =========================================================================
TEST_PROCEDURE(MoveCalculator_bishop_moves) {
  TEST_START
  Board board("k7/8/8/8/3B4/8/8/7K w - - 0 29");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 16u);
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "d4", "c3b2a1c5b6a7e5f6g7h8e3f2g1");
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "h1", "g1g2h2");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_rook_moves) {
  TEST_START
  Board board("k7/8/8/8/4r3/8/8/7K b q c7 1 15");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 17u);
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "e4", "e3e2e1e5e6e7e8d4c4b4a4f4g4h4");
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "a8", "a7b7b8");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_queen_moves) {
  TEST_START
  Board board("1k6/8/8/3q4/8/8/7K/8 b - - 1 20");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 32u);
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "d5", "c6b7a8d6d7d8e6f7g8e5f5g5h5e4f3g2h1d4d3d2d1c4b3a2c5b5a5");
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "b8", "a8a7b7c7c8");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_knight_moves) {
  TEST_START
  Board board("1k6/8/8/8/4N3/8/7K/8 w - - 0 29");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 13u);
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "e4", "g5g3f2d2c3c5d6f6");
  VERIFY_MOVES_WITHOUT_CAPTURE(moves, "h2", "h1g1g2g3h3");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_king_moves) {
  TEST_START
  {
    Board board("8/8/8/8/8/7k/8/7K w - - 0 29");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 1u);
    VERIFY_TRUE(MovesContainMove(moves, "h1g1"));
    board.ChangeSideToMove();
    moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 3u);
    VERIFY_MOVES_WITHOUT_CAPTURE(moves, "h3", "h4g4g3");
  }
  {
    Board board("2R5/1N1k4/8/8/5K2/8/8/8 b - - 0 29");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 3u);
    VERIFY_MOVES_WITHOUT_CAPTURE(moves, "d7", "e7e6");
    VERIFY_TRUE(MovesContainMove(moves, "d7c8", true));
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_castlings) {
  TEST_START
  {
    Board board("4k3/8/8/8/8/8/8/4K2R w KQ - 0 29");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 15u);
    VERIFY_TRUE(MovesContainMove(moves, "e1g1"));
  }
  {
    Board board("4k3/8/8/8/2b5/8/8/4K2R w KQ - 0 2");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 12u);
    VERIFY_FALSE(MovesContainMove(moves, "e1g1"));
  }
  TEST_END
}

}  // unnamed namespace
