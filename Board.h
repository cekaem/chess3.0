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

class Board {
 public:
  Board(const std::string& fen);
  Board(const Board& other);
  bool IsValid() const;
 
 private:
  size_t HandleFields(const std::string& fen);
  void HandleSingleRank(const std::string& fen, const std::string& rank_str, size_t rank);
  void HandleSideToMove(const std::string& fen, size_t index);

  std::array<std::array<char, 8>, 8> fields_;
  bool white_to_move_;
  unsigned halfmove_clock_;
  unsigned fullmove_number_;
  char white_king_position_[2];
  char black_king_position_[2];
};

#endif  // BOARD_H
