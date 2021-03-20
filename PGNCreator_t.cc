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
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "e2e4", "e4"},
    {"rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1", "e7e5", "e5"},
    {"3qk3/8/8/8/8/8/8/4K3 b - - 0 1", "d8d1", "Qd1+"},
    {"8/2K5/8/8/7k/8/4p3/8 b - - 0 1", "e2e1q", "e1Q"},
    {"8/2K5/8/8/7k/8/4p3/8 b - - 0 1", "e2e1b", "e1B"},
    {"8/8/2n5/1K6/7k/8/8/8 w - - 0 1", "b5c6", "Kxc6"},
    {"K7/8/k7/8/5b2/3b4/8/8 b - - 0 1", "d3e4", "Be4#"},
    {"K7/8/k7/5b2/4Rb2/5b2/8/8 b - - 0 1", "f5e4", "B5xe4#"},
    {"K7/8/k7/5b2/4Rb2/5b2/8/8 b - - 0 1", "f3e4", "B3xe4#"},
    {"4b3/3P1P2/2K5/8/7k/8/8/8 w - - 0 1", "f7e8N", "fxe8N"},
    {"K6k/8/8/8/8/3Q1Q2/8/5Q2 w - - 0 1", "f3d1", "Qf3d1"},
    {"K6k/8/8/8/8/3Q1Q2/8/5Q2 w - - 0 1", "f1d1", "Q1d1"},
    {"K6k/8/8/8/8/3Q1Q2/8/5Q2 w - - 0 1", "d3d1", "Qdd1"},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", "e1g1", "O-O"},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", "e1c1", "O-O-O"},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", "e8g8", "O-O"},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", "e8c8", "O-O-O"},
    {"4k3/8/8/Pp6/8/8/8/4K3 w KQkq b6 0 1", "a5b6", "axb6"},
    {"4k3/8/8/8/2pPp3/8/8/4K3 b KQkq d3 0 1", "c4d3", "cxd3"},
    {"4k3/8/8/8/2pPp3/8/8/4K3 b KQkq d3 0 1", "e4d3", "exd3"}
  };

  MoveCalculator calculator;
  PGNCreator fen_creator;

  for (const auto&[fen, move_str, expected_notation]: cases) {
    Board board(fen);
    auto moves = calculator.CalculateAllMoves(board);
    auto iter = std::find_if(moves.begin(), moves.end(), [move_str](const Move& move) -> bool {
      if (move_str.length() == 5u && move_str[4] != move.promotion_to) {
        return false;
      }
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
