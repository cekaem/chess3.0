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
  static char INVALID;

  bool IsInvalid() const {
    return rank == INVALID || file == INVALID;
  }

  bool operator==(const Square& other) const;
  bool operator!=(const Square& other) const;

  char rank{INVALID};
  char file{INVALID};
};

class Board {
 public:
  Board(const std::string& fen);
  Board(const Board& other);
  bool IsValid() const;
  char& at(size_t file, size_t rank) { return squares_[rank][file]; }
  char at(size_t file, size_t rank) const { return squares_[rank][file]; }
  char at(const char* square) const;
  bool CanCastle(Castling c) const { return castlings_[static_cast<size_t>(c)]; }
  Square EnPassantTargetSquare() const { return en_passant_target_square_; }
  Square KingPosition(bool white) const;
  unsigned HalfMoveClock() const { return halfmove_clock_; }
  unsigned FullMoveNumber() const { return fullmove_number_; }
 
 private:
  size_t HandleFields(const std::string& fen);
  void HandleSingleRank(const std::string& fen, const std::string& rank_str, size_t rank);
  size_t HandleSideToMove(const std::string& fen, size_t index);
  size_t HandleCastlings(const std::string& fen, size_t index);
  size_t HandleEnPassantTargetSquare(const std::string& fen, size_t index);
  size_t HandleHalfMoveClock(const std::string& fen, size_t index);
  void HandleFullMoveNumber(const std::string& fen, size_t index);

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
