#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <iostream>
#include <string>

struct InvalidFENException {
  InvalidFENException(const std::string& f, const std::string msg)
    : fen(f), error_message(msg) {}
  const std::string fen;
  const std::string error_message;
};

enum class Castling {
  K,
  Q,
  k,
  q,
  LAST
};

struct Square {
  static const unsigned char INVALID = '9';

  Square() {}
  Square(size_t _x, size_t _y) : x(_x), y(_y) {}
  Square(const std::string& square);

  void Invalidate();

  bool IsInvalid() const {
    return x == INVALID || y == INVALID;
  }

  bool operator==(const Square& other) const;
  bool operator!=(const Square& other) const;

  unsigned char x{INVALID};
  unsigned char y{INVALID};
};

std::ostream& operator<<(std::ostream& os, const Square& square);

class Board {
 public:
  Board(const std::string& fen);
  Board(const Board& other) = delete;
  Board(Board&& other) = delete;
  Board& operator=(const Board& board) = delete;
  bool IsKingInCheck(bool white) const;
  char& at(size_t x, size_t y) { return squares_[x][y]; }
  char& at(const Square& square) { return squares_[square.x][square.y]; }
  char at(const Square& square) const { return squares_[square.x][square.y]; }
  char at(size_t x, size_t y) const { return squares_[x][y]; }
  char at(const char* square) const;
  bool CanCastle(Castling c) const { return castlings_[static_cast<size_t>(c)]; }
  Square EnPassantTargetSquare() const { return en_passant_target_square_; }
  Square KingPosition(bool white) const;
  unsigned HalfMoveClock() const { return halfmove_clock_; }
  unsigned FullMoveNumber() const { return fullmove_number_; }
  bool WhiteToMove() const { return white_to_move_; }
  void ChangeSideToMove() { white_to_move_ = !white_to_move_; }
  void IncrementFullMoveNumber() { ++fullmove_number_; }
  void ResetHalfMoveClock() { halfmove_clock_ = 0u; }
  void IncrementHalfMoveClock() { ++halfmove_clock_; }
  void SetHalfMoveClock(unsigned value) { halfmove_clock_ = value; }
  void UnsetCanCastle(Castling c) { castlings_[static_cast<size_t>(c)] = false; }
  void SetEnPassantTargetSquare(Square s) { en_passant_target_square_ = s; }
  void InvalidateEnPassantTargetSquare() { en_passant_target_square_.Invalidate(); }
  void SetKingPosition(bool white, size_t x, size_t y);
  std::string CreateFEN() const;
 
 private:
  size_t HandleFields(const std::string& fen);
  void HandleSingleRank(const std::string& fen, const std::string& rank_str, size_t rank);
  size_t HandleSideToMove(const std::string& fen, size_t index);
  size_t HandleCastlings(const std::string& fen, size_t index);
  size_t HandleEnPassantTargetSquare(const std::string& fen, size_t index);
  size_t HandleHalfMoveClock(const std::string& fen, size_t index);
  void HandleFullMoveNumber(const std::string& fen, size_t index);
  bool IsKingInCheckHelper(const Square& starting_square, bool white, int x_offset, int y_offset) const;
  bool IsFigureAtGivenCoordinates(int x, int y, char figure) const;
  std::string RankToFEN(size_t rank) const;
  std::string CastlingsToFEN() const;
  std::string EnPassantTargetSquareToFEN() const;

  std::array<std::array<char, 8>, 8> squares_;
  bool white_to_move_;
  unsigned short halfmove_clock_;
  unsigned short fullmove_number_;
  Square white_king_position_;
  Square black_king_position_;
  Square en_passant_target_square_;
  bool castlings_[static_cast<size_t>(Castling::LAST)];
};

bool operator==(const Board& b1, const Board& b2);

#endif  // BOARD_H
