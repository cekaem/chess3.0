#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>

struct WrongFenException {
  WrongFenException(const std::string& f, const std::string msg)
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
  void HandleFields(const std::string& fen);

  std::array<std::array<char, 8>, 8> fields_;
  bool white_to_move_;
  unsigned halfmove_clock_;
  unsigned fullmove_number_;
};

#endif  // BOARD_H
