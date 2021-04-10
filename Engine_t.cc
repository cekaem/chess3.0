/* Component tests for class Engine */

#include <string>
#include <tuple>
#include <vector>

#include "Board.h"
#include "Engine.h"
#include "MoveCalculator.h"
#include "Types.h"
#include "utils/Test.h"


namespace {

#define COORDINATES_FROM_STRING(str)\
  const size_t old_x = str[0] - 'a'; \
  const size_t old_y = str[1] - '1'; \
  const size_t new_x = str[2] - 'a'; \
  const size_t new_y = str[3] - '1';

bool MovesAreEqual(const Move& move, const std::string& expected_move) {
  COORDINATES_FROM_STRING(expected_move);
  char promotion_to = 0x0;
  if (expected_move.size() == 5u) {
    promotion_to = expected_move[4u];
  }
  return move.old_square.x == old_x && move.old_square.y == old_y &&
         move.new_square.x == new_x && move.new_square.y == new_y &&
         move.promotion_to == promotion_to;
}

// ===============================================================

TEST_PROCEDURE(Engine_exception_is_thrown_when_no_moves) {
  TEST_START
  std::vector<std::tuple<std::string, bool, GameResult>> cases = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, GameResult::NONE},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1", false, GameResult::NONE},
    {"8/8/8/8/8/3k4/8/r2K4 w - - 0 1", true, GameResult::BLACK_WON},
    {"3R2k1/5r2/7K/3B4/8/8/8/8 b - - 0 1", true, GameResult::WHITE_WON},
    {"7K/5q2/8/5k2/8/8/8/8 w - - 0 1", true, GameResult::DRAW},
    {"8/8/8/8/8/8/3R4/5K1k b - - 0 1", true, GameResult::DRAW}
  };

  Engine engine(1u);

  for (const auto&[fen, no_moves, result]: cases) {
    Board board(fen);
    try {
      engine.CalculateBestMove(board);
      VERIFY_FALSE(no_moves) << "failed for fen " << fen;
    } catch (NoMovesException& e) {
      VERIFY_TRUE(no_moves) << "failed for fen " << fen;
      VERIFY_EQUALS(e.result, result) << "failed for fen " << fen;
    }
  }
  TEST_END
}

TEST_PROCEDURE(Engine_finds_mate_in_three) {
  TEST_START
  std::vector<std::tuple<std::string, std::string>> cases = {
    {"1r3bk1/5prp/7P/p7/bQ2N3/1q6/1P1p2R1/1K2RB2 w - - 1 2", "e4f6"}
  };

  Engine engine(5u);

  for (const auto&[fen, expected_move]: cases) {
    Board board(fen);
    auto move = engine.CalculateBestMove(board);
    VERIFY_TRUE(MovesAreEqual(move, expected_move)) << "failed for fen \"" << fen << "\"; move: " << move;
  }
  TEST_END
}

TEST_PROCEDURE(Engine_finds_mate_in_two) {
  TEST_START
  std::vector<std::tuple<std::string, std::string>> cases = {
    {"7k/4Q3/8/8/8/8/7B/6K1 w - - 0 1", "h2e5"},
    {"8/1k6/8/8/2r5/1r6/6K1/8 b - - 0 1", "c4c2"},
    {"1r5k/6pp/7N/3Q4/8/8/6K1/8 w - - 0 1", "d5g8"},
    {"8/2k5/8/8/3q4/7n/6PP/1R5K b - - 0 1", "d4g1"}
  };

  Engine engine(3u);

  for (const auto&[fen, expected_move]: cases) {
    Board board(fen);
    auto move = engine.CalculateBestMove(board);
    VERIFY_TRUE(MovesAreEqual(move, expected_move)) << "failed for fen \"" << fen << "\"; move: " << move;
  }

  TEST_END
}

TEST_PROCEDURE(Engine_finds_mate_in_one) {
  TEST_START
  std::vector<std::tuple<std::string, std::string>> cases = {
    {"3k4/8/3K4/8/8/8/8/R7 w - - 0 1", "a1a8"},
    {"1r5b/8/8/8/k7/8/K1p5/8 b - - 0 1", "c2c1N"}
  };

  Engine engine(1);

  for (const auto&[fen, expected_move]: cases) {
    Board board(fen);
    auto move = engine.CalculateBestMove(board);
    VERIFY_TRUE(MovesAreEqual(move, expected_move)) << "failed for fen \"" << fen << "\"; move: " << move;
  }
  TEST_END
}

}  // unnamed namespace
