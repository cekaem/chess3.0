/* Component tests for class PGNCreator */

#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

#include "Board.h"
#include "MoveCalculator.h"
#include "PGNCreator.h"
#include "utils/Test.h"


namespace {

TEST_PROCEDURE(PGNCreator_proper_string_is_generated) {
  TEST_START
  std::vector<std::tuple<std::string, std::string, std::string>> cases = {
    {"K6k/8/8/8/8/3Q1Q2/8/5Q2 w - - 0 1", "f3d1", "Qf3d1"},
    {"K6k/8/8/8/8/3Q1Q2/8/5Q2 w - - 0 1", "f1d1", "Q1d1"},
    {"K6k/8/8/8/8/3Q1Q2/8/5Q2 w - - 0 1", "d3d1", "Qdd1"}
  };

  MoveCalculator calculator;
  PGNCreator fen_creator;

  for (const auto&[fen, move_str, expected_notation]: cases) {
    Board board(fen);
    auto moves = calculator.CalculateAllMoves(board);
    auto iter = std::find_if(moves.begin(), moves.end(), [move_str](const Move& move) -> bool {
      return static_cast<size_t>(move_str[0] - 'a') == move.old_x &&
             static_cast<size_t>(move_str[1] - '1') == move.old_y &&
             static_cast<size_t>(move_str[2] - 'a') == move.new_x &&
             static_cast<size_t>(move_str[3] - '1') == move.new_y;
    });
    VERIFY_TRUE(iter != moves.end()) << "failed for fen \"" << fen << "\" and move " << move_str;
    const std::string notation = fen_creator.AddMove(board, *iter);
    VERIFY_EQUALS(notation, expected_notation) << "failed for fen \"" << fen << "\" and move " << move_str;
  }

  TEST_END
}

}  // unnamed namespace
