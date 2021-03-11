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
  static char INVALID;

  bool IsInvalid() const {
    return rank == INVALID || file == INVALID;
  }

  char rank{INVALID};
  char file{INVALID};
};

class Board {
 public:
  Board(const std::string& fen);
  Board(const Board& other);
  bool IsValid() const;
 
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
