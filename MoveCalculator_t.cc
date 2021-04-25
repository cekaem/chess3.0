#include "Board.h"
#include "MoveCalculator.h"
#include "utils/Test.h"

#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <utility>


namespace {

#define COORDINATES_FROM_STRING(str)\
  const size_t old_x = str[0] - 'a'; \
  const size_t old_y = str[1] - '1'; \
  const size_t new_x = str[2] - 'a'; \
  const size_t new_y = str[3] - '1';

SerializedMove SerializedMoveFromString(const std::string& str) {
  COORDINATES_FROM_STRING(str);
  char promotion_to = 0x0;
  if (str.length() == 5) {
    promotion_to = str[4];
  }
  return SerializedMove(old_x, old_y, new_x, new_y, promotion_to);
}

bool MovesContainMove(const std::vector<SerializedMove> moves,
                               const std::string&  move_str) {
  SerializedMove move = SerializedMoveFromString(move_str);
  bool result = false;
  for (const auto& m: moves) {
    if (m.data == move.data) {
      result = true;
      break;
    }
  }
  return result;
}

#define VERIFY_MOVES_CONTAIN_MOVE(moves, move) \
  VERIFY_TRUE(MovesContainMove(moves, move)) << "failed for move " << move << " "

#define VERIFY_MOVES_DOES_NOT_CONTAIN_MOVE(moves, move) \
  VERIFY_FALSE(MovesContainMove(moves, move)) << "failed for move " << move << " "

void VERIFY_MOVES_FROM_SOURCE_SQUARE(const std::vector<SerializedMove> moves,
                                     const std::string& source_square,
                                     const std::string& destination_squares) {
  VERIFY_EQUALS(destination_squares.size() % 2, 0u);
  for (size_t i = 0; i < destination_squares.length(); i += 2) {
    const std::string move_str = source_square + destination_squares.substr(i, 2);
    VERIFY_MOVES_CONTAIN_MOVE(moves, move_str);
  }
}

void VERIFY_MOVES(const std::vector<SerializedMove>& moves, const std::string& list) {
  std::stringstream ss(list);
  std::string move;
  while(getline(ss, move, ';')) {
    VERIFY_MOVES_CONTAIN_MOVE(moves, move) << "didn't found move: " << move;
  }
}

bool VectorsAreEqual(const std::vector<Square>& vec1, const std::vector<Square>& vec2) {
  if (vec1.size() != vec2.size()) {
    return false;
  }
  for (auto square1: vec1) {
    bool found = false;
    for (auto square2: vec2) {
      if (square1 == square2) {
        found = true;
        break;
      }
    }
    if (!found) {
      return false;
    }
  }
  return true;
}

void VERIFY_FIGURES_POSITIONS_ON_BOARD(const Board& board) {
  std::array<std::vector<Square>, static_cast<size_t>(Figure::LAST)> figures_positions;
  unsigned short number_of_white_knights = 0;
  unsigned short number_of_black_knights = 0;
  for (size_t x = 0; x < 8; ++x) {
    for (size_t y = 0; y < 8; ++y) {
      switch (board.at(x, y)) {
        case 'Q':
        case 'q':
        case 'R':
        case 'r':
        case 'K':
        case 'k':
        case 'B':
        case 'b':
          figures_positions[FigureCharToInt(board.at(x, y))].push_back({x, y});
          break;
        case 'N':
          ++number_of_white_knights;
          break;
        case 'n':
          ++number_of_black_knights;
          break;
        case 'P':
        case 'p':
        case 0x0:
          break;
        default:
          assert(!"Unexpected char.");
          break;
      }
    }
  }

  for (size_t i = 0; i < static_cast<size_t>(Figure::LAST); ++i) {
    VERIFY_TRUE(VectorsAreEqual(figures_positions[i],
                                board.FiguresPositions(static_cast<Figure>(i))))
        << "failed for fen \"" << board.CreateFEN() << "\" and figure " << i;
  }
  VERIFY_EQUALS(number_of_white_knights, board.NumberOfKnights(true))
      << "failed for fen \"" << board.CreateFEN() << "\"";
  VERIFY_EQUALS(number_of_black_knights, board.NumberOfKnights(false))
      << "failed for fen \"" << board.CreateFEN() << "\"";
}

bool MovesAreEqual(const SerializedMove& serialized_move, const std::string& move_str) {
  COORDINATES_FROM_STRING(move_str);
  const Move move = serialized_move.ToMove();
  return move.old_square.x == old_x && move.old_square.y == old_y &&
         move.new_square.x == new_x && move.new_square.y == new_y;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

std::ostream& operator<<(std::ostream& os, const std::vector<SerializedMove>& moves) {
  for (const auto& move: moves) {
    os << move << std::endl;
  }
  return os;
}

#pragma GCC diagnostic pop

// =========================================================================

TEST_PROCEDURE(MoveCalculator_bishop_moves) {
  TEST_START
  Board board("k7/8/8/8/3B4/8/8/7K w - - 0 29");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 16u);
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "d4", "c3b2a1c5b6a7e5f6g7h8e3f2g1");
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "h1", "g1g2h2");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_rook_moves) {
  TEST_START
  Board board("k7/8/8/8/4r3/8/8/7K b q c7 1 15");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 17u);
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "e4", "e3e2e1e5e6e7e8d4c4b4a4f4g4h4");
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "a8", "a7b7b8");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_queen_moves) {
  TEST_START
  Board board("1k6/8/8/3q4/8/8/7K/8 b - - 1 20");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 32u);
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "d5", "c6b7a8d6d7d8e6f7g8e5f5g5h5e4f3g2h1d4d3d2d1c4b3a2c5b5a5");
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "b8", "a8a7b7c7c8");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_knight_moves) {
  TEST_START
  Board board("1k6/8/8/8/4N3/8/7K/8 w - - 0 29");
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  VERIFY_EQUALS(moves.size(), 13u);
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "e4", "g5g3f2d2c3c5d6f6");
  VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "h2", "h1g1g2g3h3");
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_pawn_moves) {
  TEST_START
  {
    Board board("8/p1p5/1pP5/3k4/8/3PK2p/4PP1P/8 w KQkq - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 6u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "d3d4");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "d3d4");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2f3");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2f4");
  }
  {
    Board board("8/p1p5/1pP5/3k4/8/3PK2p/4PP1P/8 b KQkq - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 8u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "a7a6");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "a7a5");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "b6b5");
  }
  {
    Board board("K4b2/4P3/k7/8/8/8/5p2/4R3 w - - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 21u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7e8Q");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7e8R");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7e8B");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7e8N");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7f8Q");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7f8R");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7f8B");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e7f8N");
  }
  {
    Board board("K4b2/4P3/k7/8/8/8/5p2/4R3 b - - 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 14u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2f1q");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2f1r");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2f1b");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2f1n");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2e1q");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2e1r");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2e1b");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f2e1n");
  }
  {
    Board board("K7/8/k2n4/4Pp2/1Pp5/3N4/8/8 b - b3 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 12u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "c4b3");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "c4d3");
  }
  {
    Board board("K7/8/k2n4/4Pp2/1Pp5/3N4/8/8 w - f6 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 11u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e5d6");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "e5f6");
  }
  {
    Board board("8/8/8/4rPpK/8/7k/8/8 w - g6 0 17");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 4u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f5f6");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "h5g6");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "h5h6");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "h5g5");
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
    VERIFY_MOVES_CONTAIN_MOVE(moves, "h1g1");
    board.ChangeSideToMove();
    moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 3u);
    VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "h3", "h4g4g3");
  }
  {
    Board board("2R5/1N1k4/8/8/5K2/8/8/8 b - - 0 29");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 3u);
    VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "d7", "e7e6");
    VERIFY_MOVES_CONTAIN_MOVE(moves, "d7c8");
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
    {"r3k2r/8/2B5/8/8/8/8/R3K2R b KQkq - 0 2", false, false},
    {"rn2k3/8/8/8/8/8/8/4K3 b KQkq - 0 11", false, false},
    {"r3k3/8/8/4B3/8/8/8/4K3 b KQkq - 0 11", false, true},
    {"4k3/8/8/8/8/8/8/RN2K3 w KQkq - 0 11", false, false},
    {"4k3/8/8/8/8/n7/8/R3K3 w KQkq - 0 11", false, true},
    {"4k3/8/8/8/8/2n5/8/R3K3 w KQkq - 0 11", false, false}
  };

  for (const auto& [fen, can_castle_king_side, can_castle_queen_side]: cases) {
    Board board(fen);
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    const bool white_king = board.WhiteToMove();
    const std::string king_side_castling = white_king ? "e1g1" : "e8g8";
    const std::string queen_side_castling = white_king ? "e1c1" : "e8c8";
    if (can_castle_king_side) {
      VERIFY_MOVES_CONTAIN_MOVE(moves, king_side_castling) << "failed for fen: " << fen;
    } else {
      VERIFY_MOVES_DOES_NOT_CONTAIN_MOVE(moves, king_side_castling) << "failed for fen: " << fen;
    }
    if (can_castle_queen_side) {
      VERIFY_MOVES_CONTAIN_MOVE(moves, queen_side_castling) << "failed for fen: " << fen;
    } else {
      VERIFY_MOVES_DOES_NOT_CONTAIN_MOVE(moves, queen_side_castling) << "failed for fen: " << fen;
    }
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
    VERIFY_MOVES(moves, "a2a3;a2a4;b2b3;b2b4;c2c3;c2c4;d2d3;d2d4");
    VERIFY_MOVES(moves, "e2e3;e2e4;f2f3;f2f4;g2g3;g2g4;h2h3;h2h4");
    VERIFY_MOVES(moves, "b1a3;b1c3;g1f3;g1h3");
  }
  {
    Board board("8/8/4k3/1K6/8/8/6p1/8 b - - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 12u);
    VERIFY_MOVES_FROM_SOURCE_SQUARE(moves, "e6", "f6f5e5d5d6d7e7f7");
    VERIFY_MOVES(moves, "g2g1Q;g2g1R;g2g1N;g2g1B");
  }
  {
    Board board("4k3/8/8/8/8/3B4/5PPP/3r2K1 w - - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 1u);
    VERIFY_MOVES(moves, "d3f1");
  }
  {
    Board board("8/8/8/8/8/6P1/p2k1PKP/rQ6 b - - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 7u);
    VERIFY_MOVES(moves, "a1b1;a2b1Q;a2b1R;a2b1B;a2b1N;d2c3;d2e2");
  }
  {
    Board board("7k/7b/6R1/8/7R/8/8/3K4 b - - 7 9");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 0u);
  }
  {
    Board board("8/5k2/4br2/5B2/2K5/8/8/8 w - - 0 1");
    MoveCalculator calculator;
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), 7u);
    VERIFY_MOVES_CONTAIN_MOVE(moves, "f5e6");
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_board_after_move) {
  TEST_START
  const std::vector<std::tuple<std::string, std::string, std::string>> cases = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "b1c3", "rnbqkbnr/pppppppp/8/8/8/2N5/PPPPPPPP/R1BQKBNR b KQkq - 1 1"},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "e2e4", "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"},
    {"8/6P1/8/8/2K5/5k2/8/8 w - - 0 1", "g7g8Q", "6Q1/8/8/8/2K5/5k2/8/8 b - - 0 1"},
    {"8/6P1/8/8/2K5/5k2/8/8 w - - 0 1", "g7g8N", "6N1/8/8/8/2K5/5k2/8/8 b - - 0 1"},
    {"8/8/8/3K1Pp1/8/8/7k/8 w - g6 0 1", "f5g6", "8/8/6P1/3K4/8/8/7k/8 b - - 0 1"},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", "e8g8", "r4rk1/8/8/8/8/8/8/R3K2R w KQ - 1 2"},
    {"r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1", "e8c8", "2kr3r/8/8/8/8/8/8/R3K2R w KQ - 1 2"},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", "e1g1", "r3k2r/8/8/8/8/8/8/R4RK1 b kq - 1 1"},
    {"r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", "e1c1", "r3k2r/8/8/8/8/8/8/2KR3R b kq - 1 1"},
    {"3k1K2/8/8/8/8/8/5p2/6Q1 b - - 0 1", "f2g1r", "3k1K2/8/8/8/8/8/8/6r1 w - - 0 2"},
    {"8/6k1/2n5/8/3K4/8/6B1/8 w - - 0 1", "g2c6", "8/6k1/2B5/8/3K4/8/8/8 b - - 0 1"}
  };

  for (const auto&[fen_before_move, move_str, fen_after_move]: cases) {
    Board board(fen_before_move);
    SerializedMove move = SerializedMoveFromString(move_str);
    MoveCalculator::ApplyMoveOnBoard(board, move);
    VERIFY_EQUALS(board.CreateFEN(), fen_after_move)
        << "failed for fen " << fen_before_move << " and move " << move_str;
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_en_passant_target_square_after_move) {
  TEST_START
  const std::vector<std::tuple<std::string, std::string, std::string>> with_square_after_move = {
    {"8/8/5K2/8/2p5/5k2/1P6/8 w - - 0 1", "b2b4", "b3"},
    {"8/2p5/5K2/3P4/8/5k2/8/8 b - - 0 1", "c7c5", "c6"}
  };

  const std::vector<std::string> fens_resetting_en_passant = {
    {"r3k2r/8/8/8/8/8/8/R3K2R w - b6 0 1"},
    {"r3k2r/8/8/8/8/8/8/R3K2R b - e3 0 1"},
    {"4k2b/6Q1/8/N6n/8/7q/8/B3K3 b - b3 0 1"},
    {"4k2b/6Q1/8/N6n/8/7q/8/B3K3 w - b3 0 1"},
    {"8/1P1k4/8/8/8/P6p/2p5/5K2 w - h6 0 1"},
    {"8/1P1k4/8/8/8/P6p/2p5/5K2 b - a3 0 1"}
  };

  for (const auto& [fen, move_str, en_passant_target_square]: with_square_after_move) {
    Board board(fen);
    MoveCalculator::ApplyMoveOnBoard(board, SerializedMoveFromString(move_str));
    VERIFY_EQUALS(board.EnPassantTargetSquare(), Square(en_passant_target_square)) << "failed for fen " << fen;
  }

  MoveCalculator calculator;

  for (const auto& fen: fens_resetting_en_passant) {
    Board board(fen);
    auto moves = calculator.CalculateAllMoves(board);
    for (const auto& move: moves) {
      Board b(fen);
      MoveCalculator::ApplyMoveOnBoard(b, move);
      VERIFY_TRUE(b.EnPassantTargetSquare().IsInvalid());
    }
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_full_move_number) {
  TEST_START
  const std::vector<std::tuple<std::string, unsigned, unsigned>> cases = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 7", 20u, 7u},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 7", 20u, 8u},
    {"r3kbnr/pppppp1p/8/8/8/8/P2PPPPP/RNBQK2R w KQkq - 0 99", 23u, 99u},
    {"r3kbnr/pppppp1p/8/8/8/8/P2PPPPP/RNBQK2R b KQkq - 0 99", 23u, 100u}
  };

  MoveCalculator calculator;

  for (const auto& [fen, expected_number_of_moves, expected_full_move_number]: cases) {
    Board board(fen);
    auto moves = calculator.CalculateAllMoves(board);
    VERIFY_EQUALS(moves.size(), expected_number_of_moves);
    for (const auto& move: moves) {
      Board board(fen);
      calculator.ApplyMoveOnBoard(board, move);
      VERIFY_EQUALS(board.FullMoveNumber(), expected_full_move_number)
          << "failed for fen " << fen << " and move " << move;
    }
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

  auto IsMoveInList = [](const SerializedMove& move, const std::string& moves) -> bool {
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
      Board b(fen);
      MoveCalculator::ApplyMoveOnBoard(b, move);
      VERIFY_EQUALS(b.HalfMoveClock(), expected_hmc) << "failed for fen \"" << fen << "\" and move " << move;
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

  for (const auto&[fen, move, K, Q, k, q]: cases) {
    Board board(fen);
    MoveCalculator::ApplyMoveOnBoard(board, SerializedMoveFromString(move));
    VERIFY_EQUALS(board.CanCastle(Castling::K), K) << "failed for fen " << fen;
    VERIFY_EQUALS(board.CanCastle(Castling::Q), Q) << "failed for fen " << fen;
    VERIFY_EQUALS(board.CanCastle(Castling::k), k) << "failed for fen " << fen;
    VERIFY_EQUALS(board.CanCastle(Castling::q), q) << "failed for fen " << fen;
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
      Board b(fen);
      MoveCalculator::ApplyMoveOnBoard(b, move);
      VERIFY_EQUALS(b.WhiteToMove(), expected_side_to_move) << "failed for fen \"" << fen << "\" and move " << move;
    }
  }
  TEST_END
}

TEST_PROCEDURE(MoveCalculator_apply_move) {
  TEST_START
  const std::vector<std::tuple<std::string, std::string, std::string>> cases = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "e2e4",
     "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"},
    {"8/3k4/6p1/5P2/8/8/5K2/8 b - - 0 1", "g6f5", "8/3k4/8/5p2/8/8/5K2/8 w - - 0 2"},
    {"8/3k4/8/1pP5/8/8/5K2/8 w - b6 0 1", "c5b6", "8/3k4/1P6/8/8/8/5K2/8 b - - 0 1"},
    {"8/3k2p1/8/8/8/8/5K2/8 b - b3 5 76", "g7g5", "8/3k4/8/6p1/8/8/5K2/8 w - g6 0 77"},
    {"8/3k2P1/8/8/8/8/5K2/8 w - - 0 76", "g7g8Q", "6Q1/3k4/8/8/8/8/5K2/8 b - - 0 76"},
    {"8/3k2P1/8/8/8/8/5K2/8 w - - 0 76", "g7g8R", "6R1/3k4/8/8/8/8/5K2/8 b - - 0 76"},
    {"8/3k2P1/8/8/8/8/5K2/8 w - - 0 76", "g7g8B", "6B1/3k4/8/8/8/8/5K2/8 b - - 0 76"},
    {"8/3k2P1/8/8/8/8/5K2/8 w - - 0 76", "g7g8N", "6N1/3k4/8/8/8/8/5K2/8 b - - 0 76"},
    {"8/3k4/8/8/8/8/1p3K2/8 b - - 0 76", "b2b1Q", "8/3k4/8/8/8/8/5K2/1q6 w - - 0 77"},
    {"8/3k4/8/8/8/8/1p3K2/8 b - - 0 76", "b2b1R", "8/3k4/8/8/8/8/5K2/1r6 w - - 0 77"},
    {"8/3k4/8/8/8/8/1p3K2/8 b - - 0 76", "b2b1B", "8/3k4/8/8/8/8/5K2/1b6 w - - 0 77"},
    {"8/3k4/8/8/8/8/1p3K2/8 b - - 0 76", "b2b1N", "8/3k4/8/8/8/8/5K2/1n6 w - - 0 77"},
    {"8/3k4/6B1/8/8/8/5K2/1q6 b - - 0 76", "b1g6", "8/3k4/6q1/8/8/8/5K2/8 w - - 0 77"},
    {"5r2/3k2P1/8/8/8/8/5K2/8 w - - 0 76", "g7f8N", "5N2/3k4/8/8/8/8/5K2/8 b - - 0 76"},
    {"8/3k4/8/8/8/8/4rK2/8 w - - 0 76", "f2e2", "8/3k4/8/8/8/8/4K3/8 b - - 0 76"}
  };

  for (const auto& [initial_fen, move_str, expected_fen]: cases) {
    Board board(initial_fen);
    SerializedMove move = SerializedMoveFromString(move_str);
    MoveCalculator::ApplyMoveOnBoard(board, move);
    VERIFY_EQUALS(board.CreateFEN(), expected_fen);
    VERIFY_FIGURES_POSITIONS_ON_BOARD(board);
  }
  TEST_END
}

TEST_PROCEDURE(SerializedMove_constructor) {
  TEST_START
  const std::vector<std::tuple<size_t, size_t, size_t, size_t, char, unsigned short>> cases = {
    {0, 0, 0, 0, 0x0, 0},
    {1, 0, 0, 0, 0x0, 0x2000},
    {0, 2, 0, 0, 0x0, 0x0800},
    {0, 0, 5, 0, 0x0, 0x0280},
    {0, 0, 0, 7, 0x0, 0x0070},
    {0, 0, 0, 0, 'Q', 0x000e},
    {0, 0, 0, 0, 'q', 0x000e},
    {0, 0, 0, 0, 'R', 0x000c},
    {0, 0, 0, 0, 'r', 0x000c},
    {0, 0, 0, 0, 'B', 0x000a},
    {0, 0, 0, 0, 'b', 0x000a},
    {0, 0, 0, 0, 'N', 0x0008},
    {0, 0, 0, 0, 'n', 0x0008},
    {2, 7, 1, 5, 'q', 0x5cde},
    {4, 1, 4, 3, 0x0, 0x8630}
  };

  for (const auto &[old_x, old_y, new_x, new_y, promotion_to, expected_data]: cases) {
    SerializedMove serialized_move(old_x, old_y, new_x, new_y, promotion_to);
    VERIFY_EQUALS(serialized_move.data, expected_data) << "failed for expected data " << expected_data;
    Move move = serialized_move.ToMove();
    VERIFY_EQUALS(move.old_square.x, old_x) << "failed for expected data " << expected_data;
    VERIFY_EQUALS(move.old_square.y, old_y) << "failed for expected data " << expected_data;
    VERIFY_EQUALS(move.new_square.x, new_x) << "failed for expected data " << expected_data;
    VERIFY_EQUALS(move.new_square.y, new_y) << "failed for expected data " << expected_data;
    VERIFY_EQUALS(move.promotion_to, static_cast<char>(toupper(promotion_to)))
      << "failed for expected data " << expected_data;
  }
  TEST_END
}

}  // unnamed namespace
