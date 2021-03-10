#include "Board.h"

#include <iostream>

Board::Board(const std::string& fen) {
  std::array<char, 8> rank;
  rank.fill('\0');
  fields_.fill(rank);
  HandleFields(fen);
}

void Board::HandleFields(const std::string& fen) {
  size_t current_index = 0u;
  for (size_t i = 0; i < 7; ++i) {
    current_index = fen.find('/', current_index);
    if (current_index == std::string::npos) {
      throw WrongFenException(fen, "Error in piece placement section");
    }
  }
  std::cout << "ok" << std::endl;
}
