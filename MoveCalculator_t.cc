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

bool MovesContainMove(const std::vector<Move>& moves, const char* move, bool figure_beaten) {
  const size_t old_x = move[0] - 'a';
  const size_t old_y = move[1] - '1';
  const size_t new_x = move[2] - 'a';
  const size_t new_y = move[3] - '1';
  for (const auto& move: moves) {
    if (move.old_x == old_x && move.old_y == old_y &&
        move.new_x == new_x && move.new_y == new_y &&
        move.figure_beaten == figure_beaten) {
      return true;
    }
  }
  return false;
}


// =========================================================================
TEST_PROCEDURE(MoveCalculator_bishop_moves) {
  TEST_START
  Board board("k7/8/8/8/3B4/8/8/7K w - - 0 29");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 16u);
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_king_moves) {
  TEST_START
  {
    Board board("8/8/8/8/8/7k/8/7K w - - 0 29");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 1u);
    VERIFY_TRUE(MovesContainMove(moves, "h1g1", false));
    board.ChangeSideToMove();
    moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 3u);
    VERIFY_TRUE(MovesContainMove(moves, "h3h4", false));
    VERIFY_TRUE(MovesContainMove(moves, "h3g4", false));
    VERIFY_TRUE(MovesContainMove(moves, "h3g3", false));
  }
  {
    Board board("2R5/1N1k4/8/8/5K2/8/8/8 b - - 0 29");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 3u);
    VERIFY_TRUE(MovesContainMove(moves, "d7c8", true));
    VERIFY_TRUE(MovesContainMove(moves, "d7e7", false));
    VERIFY_TRUE(MovesContainMove(moves, "d7e6", false));
  }
  TEST_END
}

}  // unnamed namespace
