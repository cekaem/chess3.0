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

bool MovesContainMove(
    const std::vector<Move>& moves,
    const std::string& move,
    bool figure_captured = false,
    char promotion_to = 0x0) {
  const size_t old_x = move[0] - 'a';
  const size_t old_y = move[1] - '1';
  const size_t new_x = move[2] - 'a';
  const size_t new_y = move[3] - '1';
  for (const auto& move: moves) {
    if (move.old_x == old_x && move.old_y == old_y &&
        move.new_x == new_x && move.new_y == new_y &&
        move.figure_captured == figure_captured &&
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
