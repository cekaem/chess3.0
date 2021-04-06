/* Component tests for class PGNCreator */

#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "Board.h"
#include "MoveCalculator.h"
#include "PGNCreator.h"
#include "utils/Test.h"

namespace {

/*
std::vector<Move>::const_iterator FindMove(const std::vector<Move>& moves, const std::string& move_str) {
  assert(move_str.size() == 4u || move_str.size() == 5u);
  return std::find_if(moves.begin(), moves.end(), [move_str](const Move& move) -> bool {
    if (move_str.length() == 5u && move_str[4] != move.promotion_to) {
      return false;
    }
    return static_cast<size_t>(move_str[0] - 'a') == move.old_x &&
           static_cast<size_t>(move_str[1] - '1') == move.old_y &&
           static_cast<size_t>(move_str[2] - 'a') == move.new_x &&
           static_cast<size_t>(move_str[3] - '1') == move.new_y;
  });
}

// ========================================================================

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
    {"4k3/8/8/8/2pPp3/8/8/4K3 b KQkq d3 0 1", "e4d3", "exd3"},
    {"5k1r/8/8/8/8/8/8/4K3 b - - 0 1", "f8g8", "Kg8"}
  };

  MoveCalculator calculator;
  PGNCreator fen_creator;

  for (const auto&[fen, move_str, expected_notation]: cases) {
    Board board(fen);
    auto moves = calculator.CalculateAllMoves(board);
    auto iter = FindMove(moves, move_str);
    VERIFY_TRUE(iter != moves.end()) << "failed for fen \"" << fen << "\" and move " << move_str;
    const std::string notation = fen_creator.AddMove(board, *iter);
    VERIFY_EQUALS(notation, expected_notation) << "failed for fen \"" << fen << "\" and move " << move_str;
  }

  TEST_END
}

TEST_PROCEDURE(PGNCreator_whole_game) {
  TEST_START
  std::vector<std::tuple<std::string, std::string, GameResult, std::string>> cases = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     "e2e4;e7e5;g1f3;b8c6;f1b5",
     GameResult::DRAW,
     "[Result \"1/2-1/2\"]\n1. e4 e5 2. Nf3 Nc6 3. Bb5 1/2-1/2"},
    {"k7/3Qp3/8/5P2/2K5/8/8/8 b - - 0 40",
     "e7e5;f5e6;a8b8;e6e7;b8a8;e7e8R",
     GameResult::WHITE_WON,
     "[Result \"1-0\"]\ne5 41. fxe6 Kb8 42. e7 Ka8 43. e8R# 1-0"},
  };

  MoveCalculator calculator;

  for (const auto& [fen, moves_str, result, expected_fen]: cases) {
    PGNCreator pgn_creator;
    Board board(fen);
    std::stringstream ss(moves_str);
    std::string move_str;
    while(getline(ss, move_str, ';')) {
      auto moves = calculator.CalculateAllMoves(board);
      auto iter = FindMove(moves, move_str);
      VERIFY_TRUE(iter != moves.end()) << "failed for fen \"" << fen << "\" and move " << move_str;
      pgn_creator.AddMove(board, *iter);
      board = iter->board;
    }
    pgn_creator.GameFinished(result);
    VERIFY_EQUALS(pgn_creator.GetPGN(), expected_fen) << "failed for fen \"" << fen << "\" and moves " << moves_str;
  }
  TEST_END
}

TEST_PROCEDURE(PGNCreator_result_string) {
  TEST_START
  std::vector<std::tuple<GameResult, std::string>> cases = {
    {GameResult::NONE, ""},
    {GameResult::WHITE_WON, "1-0"},
    {GameResult::BLACK_WON, "0-1"},
    {GameResult::DRAW, "1/2-1/2"}
  };

  for (const auto&[result, result_str]: cases) {
    PGNCreator pgn_creator;
    pgn_creator.GameFinished(result);
    std::stringstream expected_string;
    if (result != GameResult::NONE) {
      expected_string << "[Result \"" << result_str << "\"]" << std::endl;
      expected_string << result_str;
    }
    VERIFY_EQUALS(pgn_creator.GetPGN(), expected_string.str()) << "failed for result " << result_str;
  }

  TEST_END
}
*/
}  // unnamed namespace
