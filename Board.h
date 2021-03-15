#ifndef BOARD_H
#define BOARD_H

#include <array>
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
  Square() {}
  Square(const char* square);
  void Invalidate();
  static size_t INVALID;

  bool IsInvalid() const {
    return rank == INVALID || file == INVALID;
  }

  bool operator==(const Square& other) const;
  bool operator!=(const Square& other) const;

  size_t rank{INVALID};
  size_t file{INVALID};
};

class Board {
 public:
  Board(const std::string& fen);
  Board(const Board& other) = default;
  Board(Board&& other) = default;
  bool IsKingInCheck(bool white) const;
  char& at(size_t x, size_t y) { return squares_[x][y]; }
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
  void IncrementHalfMoveClock() { ++halfmove_clock_; }
  void UnsetCanCastle(Castling c) { castlings_[static_cast<size_t>(c)] = false; }
  void InvalidateEnPassantTargetSquare() { en_passant_target_square_.Invalidate(); }

  void SetKingPosition(bool white, size_t x, size_t y);
 
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

  std::array<std::array<char, 8>, 8> squares_;
  bool white_to_move_;
  unsigned halfmove_clock_;
  unsigned fullmove_number_;
  Square white_king_position_;
  Square black_king_position_;
  Square en_passant_target_square_;
  bool castlings_[static_cast<size_t>(Castling::LAST)];
};

#endif  // BOARD_H
