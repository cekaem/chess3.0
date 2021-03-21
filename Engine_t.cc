/* Component tests for class Engine */

#include <string>
#include <tuple>
#include <vector>

#include "Board.h"
#include "Engine.h"
#include "Types.h"
#include "utils/Test.h"


namespace {

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

  Engine engine;

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

}  // unnamed namespace
