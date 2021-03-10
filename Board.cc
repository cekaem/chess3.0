#include "Board.h"

const char INVALID_CHAR = 9;

Board::Board(const std::string& fen) {
  std::array<char, 8> rank;
  rank.fill('\0');
  fields_.fill(rank);
  white_king_position_[0] = INVALID_CHAR;
  white_king_position_[1] = INVALID_CHAR;
  black_king_position_[0] = INVALID_CHAR;
  black_king_position_[1] = INVALID_CHAR;
  size_t index = HandleFields(fen);
  HandleSideToMove(fen, index);
}

void Board::HandleSideToMove(const std::string& fen, size_t index) {
  if (index + 1 >= fen.length()) {
    throw InvalidFENException(fen, "FEN is too short");
  }
  if (fen[index] == 'b') {
    white_to_move_ = true;
  } else if (fen[index] == 'w') {
    white_to_move_ = false;
  } else {
    throw InvalidFENException(fen, "Wrong information about side to move");
  }
  if (fen[index + 1] != ' ') {
    throw InvalidFENException(fen, "Wrong information about side to move");
  }
}

size_t Board::HandleFields(const std::string& fen) {
  size_t current_index = 0u;
  for (size_t i = 0; i < 8; ++i) {
    char separator = '/';
    if (i == 7) {
      separator = ' ';
    }
    size_t new_index = fen.find(separator, current_index);
    if (new_index == std::string::npos) {
      throw InvalidFENException(fen, "Error in piece placement section");
    }
    std::string single_rank = fen.substr(current_index, new_index - current_index);
    HandleSingleRank(fen, single_rank, 7 - i);
    current_index = new_index + 1;
  }
  if (white_king_position_[0] == INVALID_CHAR) {
    throw InvalidFENException(fen, "No white king found");
  }
  if (black_king_position_[0] == INVALID_CHAR) {
    throw InvalidFENException(fen, "No black king found");
  }
  return current_index;
}

void Board::HandleSingleRank(const std::string& fen, const std::string& rank_str, size_t rank) {
  if (rank_str.empty()) {
    throw InvalidFENException(fen, "Empty one subsection of piece placement section");
  }
  unsigned file = 0u;
  for(const char c: rank_str) {
    if (c >= '1' && c <= '8') {
      file += c - '0';
    } else if (c == 'q' || c == 'Q' || c == 'K' || c == 'k' || c == 'N' || c == 'n' ||
               c == 'R' || c == 'r' || c == 'B' || c == 'b' || c == 'P' || c == 'p') {
      fields_[rank][file] = c;
      if (c == 'K') {
        if (white_king_position_[0] != INVALID_CHAR) {
          throw InvalidFENException(fen, "Found two white kings");
        }
        white_king_position_[0] = rank;
        white_king_position_[1] = file;
      } else if (c == 'k') {
        if (black_king_position_[0] != INVALID_CHAR) {
          throw InvalidFENException(fen, "Found two black kings");
        }
        black_king_position_[0] = rank;
        black_king_position_[1] = file;
      }
      ++file;
    } else {
      throw InvalidFENException(fen, "Invalid char in piece placement section");
    }
    if (file > 8) {
      throw InvalidFENException(fen, "Invalid one subsection of piece placement section");
    }
  }
}
