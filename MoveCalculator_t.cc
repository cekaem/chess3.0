#include "Board.h"
#include "MoveCalculator.h"
#include "utils/Test.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <utility>


namespace {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

std::ostream& operator<<(std::ostream& os, const std::vector<Move>& moves) {
  for (const auto& move: moves) {
    os << move << std::endl;
  }
  return os;
}

#pragma GCC diagnostic pop

#define COORDINATES_FROM_STRING(str)\
  const size_t old_x = str[0] - 'a'; \
  const size_t old_y = str[1] - '1'; \
  const size_t new_x = str[2] - 'a'; \
  const size_t new_y = str[3] - '1';

bool operator==(const Move& m1, const Move& m2) {
  return m1.old_x == m2.old_x &&
         m1.old_y == m2.old_y &&
         m1.new_x == m2.new_x &&
         m1.new_y == m2.new_y &&
         m1.figure_captured == m2.figure_captured &&
         m1.promotion_to == m2.promotion_to;
}

bool MovesContainMove(
    const std::vector<Move>& moves,
    const Move& move) {
  for (const auto& m: moves) {
    if (m == move) {
      return true;
    }
  }
  return false;
}

bool MovesAreEqual(const Move& move, const std::string& move_str) {
  COORDINATES_FROM_STRING(move_str);
  return move.old_x == old_x && move.old_y == old_y &&
         move.new_x == new_x && move.new_y == new_y;
}

bool MovesContainMove(
    const std::vector<Move>& moves,
    const std::string& move_str,
    bool figure_captured = false,
    char promotion_to = 0x0) {
  for (const auto& move: moves) {
    if (MovesAreEqual(move, move_str) &&
        move.promotion_to == promotion_to) {
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

#define VERIFY_MOVES(moves, list) VerifyMoves(moves, list)

Move CreateMoveFromString(const std::string& str) {
  assert(str.length() == 5u || str.length() == 6u);
  assert(str[2] == '-' || str[2] == 'x');
  const size_t old_x = str[0] - 'a';
  const size_t old_y = str[1] - '1';
  const size_t new_x = str[3] - 'a';
  const size_t new_y = str[4] - '1';
  const bool figure_captured = (str[2] == 'x');
  char promotion_to = 0x0;
  if (str.length() > 5u) {
    switch (str[5]) {
      case 'Q':
      case 'q':
      case 'R':
      case 'r':
      case 'B':
      case 'b':
      case 'N':
      case 'n':
        promotion_to = str[5];
        break;
      default:
        assert(!"Unexpected char");
        break;
    }
  }
  Board fake_board("4K1k1/8/8/8/8/8/8/8 w - - 0 1");
  Move result({std::move(fake_board), old_x, old_y, new_x, new_y, promotion_to, figure_captured});
  return result;
}

void VerifyMoves(const std::vector<Move>& moves, const std::string& list) {
  std::stringstream ss(list);
  std::string move_str;
  while(getline(ss, move_str, ';')) {
    Move move = CreateMoveFromString(move_str);
    VERIFY_TRUE(MovesContainMove(moves, move)) << "didn't found move: " << move_str;
  }
}

Board FindBoardForMove(const std::vector<Move>& moves, const std::string move) {
  COORDINATES_FROM_STRING(move);
  char promotion_to = 0x0;
  if (move.length() == 5u) {
    promotion_to = move[4u];
  }
  for (const auto& m: moves) {
    if (m.old_x == old_x && m.old_y == old_y &&
        m.new_x == new_x && m.new_y == new_y &&
        m.promotion_to == promotion_to) {
      return m.board;
    }
  }
  NOT_REACHED(move);
  Board dummy_board("");
  return dummy_board;
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

TEST_PROCEDURE(MoveCalculator_pawn_moves) {
  TEST_START
  {
    Board board("8/p1p5/1pP5/3k4/8/3PK2p/4PP1P/8 w KQkq - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 6u);
    VERIFY_TRUE(MovesContainMove(moves, "d3d4"));
    VERIFY_TRUE(MovesContainMove(moves, "f2f3"));
    VERIFY_TRUE(MovesContainMove(moves, "f2f4"));
  }
  {
    Board board("8/p1p5/1pP5/3k4/8/3PK2p/4PP1P/8 b KQkq - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 8u);
    VERIFY_TRUE(MovesContainMove(moves, "a7a6"));
    VERIFY_TRUE(MovesContainMove(moves, "a7a5"));
    VERIFY_TRUE(MovesContainMove(moves, "b6b5"));
  }
  {
    Board board("K4b2/4P3/k7/8/8/8/5p2/4R3 w - - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 21u);
    VERIFY_TRUE(MovesContainMove(moves, "e7e8", false, 'Q'));
    VERIFY_TRUE(MovesContainMove(moves, "e7e8", false, 'R'));
    VERIFY_TRUE(MovesContainMove(moves, "e7e8", false, 'B'));
    VERIFY_TRUE(MovesContainMove(moves, "e7e8", false, 'N'));
    VERIFY_TRUE(MovesContainMove(moves, "e7f8", true, 'Q'));
    VERIFY_TRUE(MovesContainMove(moves, "e7f8", true, 'R'));
    VERIFY_TRUE(MovesContainMove(moves, "e7f8", true, 'B'));
    VERIFY_TRUE(MovesContainMove(moves, "e7f8", true, 'N'));
  }
  {
    Board board("K4b2/4P3/k7/8/8/8/5p2/4R3 b - - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 14u);
    VERIFY_TRUE(MovesContainMove(moves, "f2f1", false, 'q'));
    VERIFY_TRUE(MovesContainMove(moves, "f2f1", false, 'r'));
    VERIFY_TRUE(MovesContainMove(moves, "f2f1", false, 'b'));
    VERIFY_TRUE(MovesContainMove(moves, "f2f1", false, 'n'));
    VERIFY_TRUE(MovesContainMove(moves, "f2e1", true, 'q'));
    VERIFY_TRUE(MovesContainMove(moves, "f2e1", true, 'r'));
    VERIFY_TRUE(MovesContainMove(moves, "f2e1", true, 'b'));
    VERIFY_TRUE(MovesContainMove(moves, "f2e1", true, 'n'));
  }
  {
    Board board("K7/8/k2n4/4Pp2/1Pp5/3N4/8/8 b - b3 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 12u);
    VERIFY_TRUE(MovesContainMove(moves, "c4b3", true));
    VERIFY_TRUE(MovesContainMove(moves, "c4d3", true));
  }
  {
    Board board("K7/8/k2n4/4Pp2/1Pp5/3N4/8/8 w - f6 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 11u);
    VERIFY_TRUE(MovesContainMove(moves, "e5d6", true));
    VERIFY_TRUE(MovesContainMove(moves, "e5f6", true));
  }
  {
    Board board("8/8/8/4rPpK/8/7k/8/8 w - g6 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 4u);
    VERIFY_TRUE(MovesContainMove(moves, "f5f6"));
    VERIFY_TRUE(MovesContainMove(moves, "h5g6"));
    VERIFY_TRUE(MovesContainMove(moves, "h5h6"));
    VERIFY_TRUE(MovesContainMove(moves, "h5g5", true));
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_various_cases) {
  TEST_START
  {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 20u);
    VERIFY_MOVES(moves, "a2-a3;a2-a4;b2-b3;b2-b4;c2-c3;c2-c4;d2-d3;d2-d4");
    VERIFY_MOVES(moves, "e2-e3;e2-e4;f2-f3;f2-f4;g2-g3;g2-g4;h2-h3;h2-h4");
    VERIFY_MOVES(moves, "b1-a3;b1-c3;g1-f3;g1-h3");
  }
  {
    Board board("8/8/4k3/1K6/8/8/6p1/8 b - - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 12u);
    VERIFY_MOVES_WITHOUT_CAPTURE(moves, "e6", "f6f5e5d5d6d7e7f7");
    VERIFY_MOVES(moves, "g2-g1q;g2-g1r;g2-g1n;g2-g1b");
  }
  {
    Board board("4k3/8/8/8/8/3B4/5PPP/3r2K1 w - - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 1u);
    VERIFY_MOVES(moves, "d3-f1");
  }
  {
    Board board("8/8/8/8/8/6P1/p2k1PKP/rQ6 b - - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 7u);
    VERIFY_MOVES(moves, "a1xb1;a2xb1q;a2xb1r;a2xb1b;a2xb1n;d2-c3;d2-e2");
  }
  {
    Board board("7k/7b/6R1/8/7R/8/8/3K4 b - - 7 9");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 0u);
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_full_move_number) {
  TEST_START
  {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 7");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 20u);
    for (const auto& move: moves) {
      VERIFY_EQUALS(move.board.FullMoveNumber(), 7u) << "failed for move: " << move;
    }
  }
  {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 7");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 20u);
    for (const auto& move: moves) {
      VERIFY_EQUALS(move.board.FullMoveNumber(), 8u) << "failed for move: " << move;
    }
  }
  {
    Board board("r3kbnr/pppppp1p/8/8/8/8/P2PPPPP/RNBQK2R w KQkq - 0 99");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 23u);
    for (const auto& move: moves) {
      VERIFY_EQUALS(move.board.FullMoveNumber(), 99u) << "failed for move: " << move;
    }
  }
  {
    Board board("r3kbnr/pppppp1p/8/8/8/8/P2PPPPP/RNBQK2R b KQkq - 0 99");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 23u);
    for (const auto& move: moves) {
      VERIFY_EQUALS(move.board.FullMoveNumber(), 100u) << "failed for move: " << move;
    }
  }
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

TEST_PROCEDURE(MoveCalculator_half_move_clock) {
  TEST_START
  const std::vector<std::pair<std::string, std::string>> cases = {
    {"6R1/8/8/4rPp1/3K4/7k/8/8 w - g6 10 7", "d4e5f5f6g8g5f5g6"},
    {"6R1/8/8/4rPp1/3K4/7k/8/8 b - g6 16 8", "e5f5g5g4"},
    {"8/P1N5/4pk2/8/8/3q4/3K3P/8 w - g6 20 7", "a7a8c7e6d2d3h2h3h2h4"},
    {"n3q3/7p/6p1/k7/Pp6/8/5K2/1b6 b - a3 1 7", "a5a4g6g5h7h6h7h5b4b3b4a3e8a4"},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq a3 5 70", "a1a8h1h8"}
  };

  auto IsMoveInList = [](const Move& move, const std::string& moves) -> bool {
    for (size_t i = 0; i < moves.length(); i += 4) {
      const std::string move_str = moves.substr(i, 4);
      if (MovesAreEqual(move, move_str)) {
        return true;
      }
    }
    return false;
  };

  for (const auto&[fen, moves_reseting_hmc]: cases) {
    Board board(fen);
    const size_t incremented_hmc = board.HalfMoveClock() + 1u;
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    for (const auto& move: moves) {
      size_t expected_hmc = 0u;
      if (!IsMoveInList(move, moves_reseting_hmc)) {
        expected_hmc = incremented_hmc;
      }
      VERIFY_EQUALS(move.board.HalfMoveClock(), expected_hmc) << "failed for fen \"" << fen << "\" and move " << move;
    }
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_castlings_reset) {
  TEST_START
  const std::vector<std::tuple<std::string, std::string, bool, bool, bool, bool>> cases = {
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq a6 0 7", "e1g1", false, false, true, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq a6 0 7", "e1c1", false, false, true, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq a6 0 7", "e8g8", true, true, false, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq a6 0 7", "e8c8", true, true, false, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq a6 0 7", "e1f1", false, false, true, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq a6 0 7", "h1h2", false, true, true, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq a6 0 7", "a1b1", true, false, true, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq a6 0 7", "e8e7", true, true, false, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq a6 0 7", "a8a5", true, true, true, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq a6 0 7", "h8f8", true, true, false, true},
    {"r3k2r/8/8/8/8/5Q2/8/R3K2R w KQkq a6 0 7", "f3f4", true, true, true, true},
    {"r3k2r/p7/8/8/8/8/8/R3K2R b KQkq a3 0 7", "a7a5", true, true, true, true},
    {"r3k2r/8/8/8/3R4/8/8/R3K2R w KQkq a6 0 7", "d4e4", true, true, true, true}
  };

  MoveCalculator calculator;

  for (const auto&[fen, move, K, Q, k, q]: cases) {
    auto moves = calculator.CalculateAllMoves(fen);
    Board board_after_move = FindBoardForMove(moves, move);
    VERIFY_EQUALS(board_after_move.CanCastle(Castling::K), K) << "failed for fen " << fen;
    VERIFY_EQUALS(board_after_move.CanCastle(Castling::Q), Q) << "failed for fen " << fen;
    VERIFY_EQUALS(board_after_move.CanCastle(Castling::k), k) << "failed for fen " << fen;
    VERIFY_EQUALS(board_after_move.CanCastle(Castling::q), q) << "failed for fen " << fen;
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_figures_placement_after_move) {
  TEST_START
  const std::vector<std::tuple<std::string, std::string, std::string>> cases = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "b1c3", "rnbqkbnr/pppppppp/8/8/8/2N5/PPPPPPPP/R1BQKBNR"},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "e2e4", "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"},
    {"8/6P1/8/8/2K5/5k2/8/8 w - - 0 1", "g7g8Q", "6Q1/8/8/8/2K5/5k2/8/8"},
    {"8/6P1/8/8/2K5/5k2/8/8 w - - 0 1", "g7g8N", "6N1/8/8/8/2K5/5k2/8/8"},
    {"8/8/8/3K1Pp1/8/8/7k/8 w - g6 0 1", "f5g6", "8/8/6P1/3K4/8/8/7k/8"},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", "e8g8", "r4rk1/8/8/8/8/8/8/R3K2R"},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", "e8c8", "2kr3r/8/8/8/8/8/8/R3K2R"},
    {"3k1K2/8/8/8/8/8/5p2/6Q1 b - - 0 1", "f2g1r", "3k1K2/8/8/8/8/8/8/6r1"},
    {"8/6k1/2n5/8/3K4/8/6B1/8 w - - 0 1", "g2c6", "8/6k1/2B5/8/3K4/8/8/8"}
  };

  MoveCalculator calculator;

  for (const auto&[fen, move, placement_after_move]: cases) {
    auto moves = calculator.CalculateAllMoves(fen);
    Board board_after_move = FindBoardForMove(moves, move);
    const std::string new_fen = placement_after_move + " w - - 0 1";
    Board expected_board(new_fen);
    for (size_t x = 0; x < 8u; ++x) {
      for (size_t y = 0; y < 8u; ++y) {
        VERIFY_EQUALS(board_after_move.at(x, y), expected_board.at(x, y)) \
          << "failed for fen \"" << fen << "\" at (" << x << ", " << y << ")";
      }
    }
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_side_to_move_after_move) {
  TEST_START
  std::vector<std::string> fens = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
    "2k4r/8/7b/8/1Pp5/8/4K3/7q b - b3 0 1",
    "r3k2r/8/8/8/8/8/8/R3K2R w KQkq b3 0 1",
    "r3k2r/8/8/8/8/8/8/R3K2R b KQkq b3 0 1"
  };

  MoveCalculator calculator;

  for (const auto& fen: fens) {
    Board board(fen);
    const bool expected_side_to_move = !board.WhiteToMove();
    auto moves = calculator.CalculateAllMoves(fen);
    for (const auto& move: moves) {
      VERIFY_EQUALS(move.board.WhiteToMove(), expected_side_to_move) << "failed for fen \"" << fen << "\" and move " << move;
    }
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_en_passant_target_square_after_move) {
  TEST_START
  std::vector<std::tuple<std::string, std::string, std::string>> cases = {
    {"8/8/5K2/8/2p5/5k2/1P6/8 w - - 0 1", "b2b4", "b3"},
    {"8/8/5K2/2pP4/8/5k2/8/8 b - - 0 1", "c7c5", "c6"},
    // Below are test cases in which all moves should invalidate en passant target square
    // so I provided dummy move and dummy expected en-passant target square.
    {"r3k2r/8/8/8/8/8/8/R3K2R w - b6 0 1", "a1a1", ""},
    {"r3k2r/8/8/8/8/8/8/R3K2R b - e3 0 1", "a1a1", ""},
    {"4k2b/6Q1/8/N6n/8/7q/8/B3K3 b - b3 0 1", "a1a1", ""},
    {"4k2b/6Q1/8/N6n/8/7q/8/B3K3 w - b3 0 1", "a1a1", ""},
    {"8/1P1k4/8/8/8/P6p/2p5/5K2 w - h6 0 1", "a1a1", ""},
    {"8/1P1k4/8/8/8/P6p/2p5/5K2 b - a3 0 1", "a1a1", ""}
  };

  MoveCalculator calculator;

  for (const auto& [fen, move_str, en_passant_target_square]: cases) {
    auto moves = calculator.CalculateAllMoves(fen);
    for (const auto& move: moves) {
      if (MovesAreEqual(move, move_str)) {
        VERIFY_EQUALS(move.board.EnPassantTargetSquare(), Square(en_passant_target_square)) << "failed for fen " << fen;
      } else {
        VERIFY_TRUE(move.board.EnPassantTargetSquare().IsInvalid())  << "failed for fen \"" << fen << "\" and move " << move;
      }
    }
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_castlings) {
  TEST_START
  const std::vector<std::tuple<std::string, bool, bool>> cases = {
    {"4k3/8/8/8/8/8/8/4K2R w KQ - 0 29", true, false},
    {"4k3/8/8/8/2b5/8/8/4K2R w KQ - 0 2", false, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 2", true, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 2", true, true},
    {"r3k2r/8/8/8/8/8/8/R3K1NR w KQkq - 0 2", false, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R w K - 0 2", true, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R w Q - 0 2", false, true},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQ - 0 2", false, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R b k - 0 2", true, false},
    {"r3k2r/8/8/8/8/8/8/R3K2R b q - 0 2", false, true},
    {"r2k3r/8/8/8/8/8/8/R4K1R w KQkq - 0 2", false, false},
    {"r2k3r/8/8/8/8/8/8/R4K1R b KQkq - 0 2", false, false},
    {"r1n1k2r/8/8/8/8/8/8/R3K1NR w KQkq - 0 2", false, true},
    {"r1n1k2r/8/8/8/8/8/8/R3K1NR b KQkq - 0 2", true, false},
    {"r3k3/7r/8/8/8/8/8/1R2K2R w KQkq - 0 2", true, false},
    {"r3k1r1/8/8/8/8/8/8/1R2K2R b KQkq - 0 2", false, true},
    {"r3k2r/8/5R2/8/8/2r5/8/R3K2R w KQkq - 0 2", true, false},
    {"r3k2r/8/5R2/8/8/2r5/8/R3K2R b KQkq - 0 2", false, true},
    {"r3k2r/8/8/8/8/4r3/8/R3K2R w KQkq - 0 2", false, false},
    {"r3k2r/8/2B5/8/8/8/8/R3K2R b KQkq - 0 2", false, false}
  };

  for (const auto& [fen, can_castle_king_side, can_castle_queen_side]: cases) {
    Board board(fen);
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    const bool white_king = board.WhiteToMove();
    const std::string king_side_castling = white_king ? "e1g1" : "e8g8";
    const std::string queen_side_castling = white_king ? "e1c1" : "e8c8";
    if (can_castle_king_side) {
      VERIFY_TRUE(MovesContainMove(moves, king_side_castling)) << "failed for fen: " << fen;
    } else {
      VERIFY_FALSE(MovesContainMove(moves, king_side_castling)) << "failed for fen: " << fen;
    }
    if (can_castle_queen_side) {
      VERIFY_TRUE(MovesContainMove(moves, queen_side_castling)) << "failed for fen: " << fen;
    } else {
      VERIFY_FALSE(MovesContainMove(moves, queen_side_castling)) << "failed for fen: " << fen;
    }
  }
  TEST_END
}

}  // unnamed namespace
