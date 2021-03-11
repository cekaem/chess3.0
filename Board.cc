#include "Board.h"
#include "utils/Utils.h"

char Square::INVALID = -1;

Board::Board(const std::string& fen) {
  size_t index = HandleFields(fen);
  index = HandleSideToMove(fen, index);
  index = HandleCastlings(fen, index);
  index = HandleEnPassantTargetSquare(fen, index);
  index = HandleHalfMoveClock(fen, index);
  HandleFullMoveNumber(fen, index);
}

void Board::HandleFullMoveNumber(const std::string& fen, size_t index) {
  const std::string& full_move_number_str = fen.substr(index);
  if (!utils::str_2_uint(full_move_number_str, fullmove_number_)) {
    throw InvalidFENException(fen, "Invalid full move number");
  }
}

size_t Board::HandleHalfMoveClock(const std::string& fen, size_t index) {
  size_t end_index = fen.find(' ', index);
  if (end_index == std::string::npos) {
    throw InvalidFENException(fen, "Error in half move clock section");
  }
  const std::string& half_move_clock_str = fen.substr(index, end_index - index);
  if (!utils::str_2_uint(half_move_clock_str, halfmove_clock_)) {
    throw InvalidFENException(fen, "Invalid half move clock");
  }
  return end_index + 1;
}

size_t Board::HandleEnPassantTargetSquare(const std::string& fen, size_t index) {
  size_t end_index = fen.find(' ', index);
  if (end_index == index + 1) {
    if (fen[index] != '-') {
      throw InvalidFENException(fen, "Error in en passant target square section");
    }
  } else if (end_index == index + 2) {
    if (fen[index] < 'a' || fen[index] > 'h') {
      throw InvalidFENException(fen, "Invalid rank in en passant target square");
    }
    en_passant_target_square_.rank = fen[index] - 'a';
    if (fen[index + 1] < '1' || fen[index + 1] > '8') {
      throw InvalidFENException(fen, "Invalid file in en passant target square");
    }
    en_passant_target_square_.file = fen[index] - '1';
  } else {
    throw InvalidFENException(fen, "Error in en passant target square section");
  }
  return end_index + 1;
}

size_t Board::HandleCastlings(const std::string& fen, size_t index) {
  size_t end_index = fen.find(' ', index);
  if (end_index == std::string::npos) {
    throw InvalidFENException(fen, "Error in castlings section");
  }
  castlings_[static_cast<size_t>(Castling::Q)] = false;
  castlings_[static_cast<size_t>(Castling::q)] = false;
  castlings_[static_cast<size_t>(Castling::K)] = false;
  castlings_[static_cast<size_t>(Castling::k)] = false;
  for (size_t i = index; i < end_index; ++i) {
    switch (fen[i]) {
      case '-':
        if (end_index != index + 1) {
          throw InvalidFENException(fen, "Error in castlings section");
        }
        break;
      case 'Q':
        castlings_[static_cast<size_t>(Castling::Q)] = true;
        break;
      case 'q':
        castlings_[static_cast<size_t>(Castling::q)] = true;
        break;
      case 'K':
        castlings_[static_cast<size_t>(Castling::K)] = true;
        break;
      case 'k':
        castlings_[static_cast<size_t>(Castling::k)] = true;
        break;
      default:
        throw InvalidFENException(fen, "Error in castlings section");
        break;
    }
  }
  return end_index + 1;
}

size_t Board::HandleSideToMove(const std::string& fen, size_t index) {
  if (index + 2 >= fen.length()) {
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
  return index + 2;
}

size_t Board::HandleFields(const std::string& fen) {
  std::array<char, 8> rank;
  rank.fill('\0');
  squares_.fill(rank);
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
  if (white_king_position_.IsInvalid()) {
    throw InvalidFENException(fen, "No white king found");
  }
  if (black_king_position_.IsInvalid()) {
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
      squares_[rank][file] = c;
      if (c == 'K') {
        if (!white_king_position_.IsInvalid()) {
          throw InvalidFENException(fen, "Found two white kings");
        }
        white_king_position_.rank = rank;
        white_king_position_.file = file;
      } else if (c == 'k') {
        if (!black_king_position_.IsInvalid()) {
          throw InvalidFENException(fen, "Found two black kings");
        }
        black_king_position_.rank = rank;
        black_king_position_.file = file;
      }
      ++file;
    } else {
      throw InvalidFENException(fen, "Invalid char in piece placement section");
    }
    if (file > 8) {
      break;
    }
  }
  if (file != 8) {
    throw InvalidFENException(fen, "Invalid one subsection of piece placement section");
  }
}
