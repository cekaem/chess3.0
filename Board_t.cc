/* Component tests for class Board */

#include <iostream>

#include "Board.h"
#include "utils/Test.h"

namespace {

std::ostream& operator<<(std::ostream& os, const Square& square) {
  os << static_cast<char>(square.file + 'a') << static_cast<char>(square.rank + '1');
  return os;
}


//================================================================================

TEST_PROCEDURE(Board_fen_constructor_valid_fens) {
  TEST_START
  {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    VERIFY_EQUALS(board.at("a1"), 'R');
    VERIFY_EQUALS(board.at("d1"), 'Q');
    VERIFY_EQUALS(board.at("c2"), 'P');
    VERIFY_EQUALS(board.at("e4"), '\0');
    VERIFY_EQUALS(board.at(2, 6), 'p');
    VERIFY_EQUALS(board.at("g7"), 'p');
    VERIFY_EQUALS(board.at("h8"), 'r');
    VERIFY_EQUALS(board.at("e8"), 'k');
    VERIFY_EQUALS(board.at(2, 6), 'p');
    board.at(2, 6) = 'Q';
    VERIFY_EQUALS(board.at(2, 6), 'Q');
    VERIFY_TRUE(board.CanCastle(Castling::Q));
    VERIFY_TRUE(board.CanCastle(Castling::q));
    VERIFY_TRUE(board.CanCastle(Castling::K));
    VERIFY_TRUE(board.CanCastle(Castling::k));
    VERIFY_TRUE(board.EnPassantTargetSquare().IsInvalid());
    VERIFY_EQUALS(board.KingPosition(true), Square("e1"));
    VERIFY_EQUALS(board.KingPosition(false), Square("e8"));
    VERIFY_EQUALS(board.HalfMoveClock(), 0u);
    VERIFY_EQUALS(board.FullMoveNumber(), 1u);
  }
  {
    Board board("6kR/pppq1rB1/n2pr3/3Pp3/1PP3Q1/P3P3/6K1/7R b - - 0 29");
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

} // unnamed namespace
