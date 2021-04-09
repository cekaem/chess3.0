/* Component tests for class Board */

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "Board.h"
#include "utils/Test.h"

namespace {

TEST_PROCEDURE(Board_fen_constructor_valid_fens) {
  TEST_START
  {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    VERIFY_EQUALS(board.at("a1"), 'R');
    VERIFY_EQUALS(board.at("d1"), 'Q');
    VERIFY_EQUALS(board.at("c2"), 'P');
    VERIFY_EQUALS(board.at("e4"), '\0');
    VERIFY_EQUALS(board.at("c7"), 'p');
    VERIFY_EQUALS(board.at(2, 6), 'p');
    VERIFY_EQUALS(board.at("g7"), 'p');
    VERIFY_EQUALS(board.at("h8"), 'r');
    VERIFY_EQUALS(board.at("e8"), 'k');
    VERIFY_TRUE(board.WhiteToMove());
    VERIFY_TRUE(board.CanCastle(Castling::Q));
    VERIFY_TRUE(board.CanCastle(Castling::q));
    VERIFY_TRUE(board.CanCastle(Castling::K));
    VERIFY_TRUE(board.CanCastle(Castling::k));
    VERIFY_TRUE(board.EnPassantTargetSquare().IsInvalid());
    VERIFY_EQUALS(board.KingPosition(true), Square("e1"));
    VERIFY_EQUALS(board.KingPosition(false), Square("e8"));
    VERIFY_EQUALS(board.HalfMoveClock(), 0u);
    VERIFY_EQUALS(board.FullMoveNumber(), 1u);
    VERIFY_EQUALS(board.at(2, 6), 'p');
    board.at(2, 6) = 'Q';
    VERIFY_EQUALS(board.at(2, 6), 'Q');
  }
  {
    Board board("6kR/pppq1rB1/n2pr3/3Pp3/1PP3Q1/P3P3/6K1/7R b - - 0 29");
    VERIFY_FALSE(board.WhiteToMove());
    VERIFY_FALSE(board.CanCastle(Castling::Q));
    VERIFY_FALSE(board.CanCastle(Castling::q));
    VERIFY_FALSE(board.CanCastle(Castling::K));
    VERIFY_FALSE(board.CanCastle(Castling::k));
    VERIFY_TRUE(board.EnPassantTargetSquare().IsInvalid());
    VERIFY_EQUALS(board.KingPosition(true), Square("g2"));
    VERIFY_EQUALS(board.KingPosition(false), Square("g8"));
    VERIFY_EQUALS(board.HalfMoveClock(), 0u);
    VERIFY_EQUALS(board.FullMoveNumber(), 29u);
  }
  {
    Board board("r1bqkbnr/ppppp1pp/2n5/4Pp2/8/8/PPPP1PPP/RNBQKBNR w Kq f6 11 30");
    VERIFY_TRUE(board.WhiteToMove());
    VERIFY_FALSE(board.CanCastle(Castling::Q));
    VERIFY_TRUE(board.CanCastle(Castling::q));
    VERIFY_TRUE(board.CanCastle(Castling::K));
    VERIFY_FALSE(board.CanCastle(Castling::k));
    VERIFY_FALSE(board.EnPassantTargetSquare().IsInvalid());
    VERIFY_EQUALS(board.EnPassantTargetSquare(), Square("f6"));
    VERIFY_EQUALS(board.KingPosition(true), Square("e1"));
    VERIFY_EQUALS(board.KingPosition(false), Square("e8"));
    VERIFY_EQUALS(board.HalfMoveClock(), 11u);
    VERIFY_EQUALS(board.FullMoveNumber(), 30u);
  }
  TEST_END
}

TEST_PROCEDURE(Board_fen_constructor_invalid_fens) {
  TEST_START
  std::vector<std::string> invalid_fens = {
    "",
    "r1bqkbnr//2n5/4Pp2/8/8/PPPP1PPP/RNBQKBNR w Kq f6 0 3",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBKKBNR w KQkq - 0 1",
    "rnbkkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "anbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/ppppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/ppppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR c KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR ww KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkp - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq j2 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq a9 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - d 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 q",
    "rnbqkbnr/ppppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    "r1bqkbnr/2pppp1p/p1n5/1p4p1/4P2/1P1B1N2/P1PP1PPP/RNBQK2R b KQkq b6 0 5"
  };

  for (const auto& fen: invalid_fens) {
    bool exception_was_thrown = false;
    try {
      Board board(fen);
    } catch(InvalidFENException& e) {
      exception_was_thrown = true;
    }
    if (exception_was_thrown == false) {
      NOT_REACHED(std::string("Exception InvalidFENException was not thrown for fen \"") + fen + "\"");
    }
  }
  TEST_END
}

TEST_PROCEDURE(Board_en_passant_square) {
  TEST_START
  Board board("8/8/8/3k4/8/6K1/8/8 b - b3 0 17");
  Square en_passant_square = board.EnPassantTargetSquare();
  VERIFY_EQUALS(en_passant_square.x, 1u);
  VERIFY_EQUALS(en_passant_square.y, 2u);
  TEST_END
}

TEST_PROCEDURE(Board_create_fen) {
  TEST_START
  const std::vector<std::string> fens = {
    "B1b2k1r/2pp2b1/Q6p/3Ppp2/1P2npP1/n7/R1PB1q1P/1N3KNR w - - 0 31",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQ c3 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/1PPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Qkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq a6 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 1 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0"
  };

  for (const auto& fen: fens) {
    Board board(fen);
    VERIFY_EQUALS(fen, board.CreateFEN());
  }

  TEST_END
}

TEST_PROCEDURE(Board_operator_equals) {
  TEST_START
  const std::vector<std::tuple<std::string, std::string, bool>> cases = {
    {"B1b2k1r/2pp2b1/Q6p/3Ppp2/1P2npP1/n7/R1PB1q1P/1N3KNR w - - 0 31",
     "B1b2k1r/2pp2b1/Q6p/3Ppp2/1P2npP1/n7/R1PB1q1P/1N3KNR w - - 0 31",
     true},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     true},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQ c3 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQ c3 0 1",
     true},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/1PPPPPPP/RNBQKBNR w KQkq - 0 1",
     false},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Qkq - 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     false},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1",
     false},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq a6 0 1",
     false},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 1 1",
     false},
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
     "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0",
     false}
  };

  for (const auto&[fen1, fen2, are_equal]: cases) {
    Board b1(fen1);
    Board b2(fen2);
    VERIFY_EQUALS(b1 == b2, are_equal) << "failed for fen1 \"" << fen1 << "\" and fen2 \"" << fen2 << "\"";
  }
  TEST_END
}

TEST_PROCEDURE(Board_clone) {
  TEST_START
  const std::vector<std::string> fens = {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "4K3/8/8/8/2Pp4/7k/8/8 b - c3 0 1",
    "4K3/8/8/8/2qQ4/7k/8/8 w Kq - 10 15"
  };

  for (const auto& fen: fens) {
    Board board(fen);
    Board clone = board.Clone();
    VERIFY_EQUALS(board, clone);
  }
  TEST_END
}

TEST_PROCEDURE(Board_IsKingInCheck) {
  TEST_START
  const std::vector<std::tuple<std::string, bool, bool>> cases = {
    {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, false},
    {"6kR/pppq1rB1/n2pr3/3Pp3/1PP3Q1/P3P3/6K1/7R b - - 0 29", false, true},
    {"2k1R3/8/8/8/8/4K3/8/8 w - - 0 1", false, true},
    {"8/8/8/8/4k3/8/4K3/8 b - - 0 29", false, false},
    {"8/8/8/4K3/4k3/8/8/8 b - - 0 29", true, true},
    {"8/2q5/3K2R1/8/8/6n1/6k1/8 b - - 0 29", true, false},
    {"5KR1/8/8/8/8/b7/8/6k1 b - - 0 29", true, true},
    {"5K2/4R3/8/8/8/b7/8/4k3 b - - 0 29", false, true},
    {"3N4/1pppp3/2k5/8/5K2/7n/8/8 b - - 0 29", true, true},
    {"8/8/6n1/4K3/8/8/8/5k2 b - - 0 29", true, false},
    {"8/8/8/4K3/6n1/8/8/5k2 b - - 0 29", true, false},
    {"8/8/8/4K3/8/5n2/8/5k2 b - - 0 29", true, false},
    {"8/8/8/4K3/8/3n4/8/5k2 b - - 0 29", true, false},
    {"8/8/6k1/4N3/8/8/7K/8 b - - 0 29", false, true},
    {"8/4N3/6k1/8/8/8/7K/8 b - - 0 29", false, true},
    {"5N2/8/6k1/8/8/8/7K/8 b - - 0 29", false, true},
    {"7N/8/6k1/8/8/8/7K/8 b - - 0 29", false, true},
    {"8/8/4k3/4P3/8/3K4/8/8 b - - 0 29", false, false},
    {"8/8/4k3/8/6P1/3K4/8/8 b - - 0 29", false, false},
    {"8/8/4k3/3P4/8/3K4/8/8 b - - 0 29", false, true},
    {"8/8/4k3/5P2/8/3K4/8/8 b - - 0 29", false, true},
    {"8/8/4k3/8/2p5/3K4/8/8 b - - 0 29", true, false},
    {"8/8/4k3/8/4p3/3K4/8/8 b - - 0 29", true, false}
  };

  for (const auto& [fen, white_in_check, black_in_check]: cases) {
    Board board(fen);
    VERIFY_EQUALS(board.IsKingInCheck(true), white_in_check) << "failed for fen: " << fen;
    VERIFY_EQUALS(board.IsKingInCheck(false), black_in_check) << "failed for fen: " << fen;
  }
  TEST_END
}

} // unnamed namespace
