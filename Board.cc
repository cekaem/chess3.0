#include "Board.h"

#include "utils/Utils.h"


size_t Square::INVALID = 9;

Square::Square(const std::string& square) {
  x = square[0] - 'a';
  y = square[1] - '1';
}

void Square::Invalidate() {
  x = Square::INVALID;
  y = Square::INVALID;
}

bool Square::operator==(const Square& other) const {
  return x == other.x && y == other.y;
}

bool Square::operator!=(const Square& other) const {
  return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Square& square) {
  if (square.IsInvalid()) {
    os << "INVALID_SQUARE";
  } else {
  os << static_cast<char>(square.x + 'a') << static_cast<char>(square.y + '1');
  }
  return os;
}

bool operator==(const Board& b1, const Board& b2) {
  for (size_t x = 0; x < 8u; ++x) {
    for (size_t y = 0; y < 8u; ++y) {
      if (b1.at(x, y) != b2.at(x, y)) {
        return false;
      }
    }
  }
  return b1.WhiteToMove() == b2.WhiteToMove() &&
         b1.HalfMoveClock() == b2.HalfMoveClock() &&
         b1.FullMoveNumber() == b2.FullMoveNumber() &&
         b1.EnPassantTargetSquare() == b2.EnPassantTargetSquare() &&
         b1.CanCastle(Castling::Q) == b2.CanCastle(Castling::Q) &&
         b1.CanCastle(Castling::K) == b2.CanCastle(Castling::K) &&
         b1.CanCastle(Castling::q) == b2.CanCastle(Castling::q) &&
         b1.CanCastle(Castling::k) == b2.CanCastle(Castling::k);
}

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
    en_passant_target_square_.x = fen[index] - 'a';
    if (fen[index + 1] < '1' || fen[index + 1] > '8') {
      throw InvalidFENException(fen, "Invalid file in en passant target square");
    }
    en_passant_target_square_.y = fen[index + 1] - '1';
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
  if (fen[index] == 'w') {
    white_to_move_ = true;
  } else if (fen[index] == 'b') {
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
      squares_[file][rank] = c;
      if (c == 'K') {
        if (!white_king_position_.IsInvalid()) {
          throw InvalidFENException(fen, "Found two white kings");
        }
        white_king_position_.x = file;
        white_king_position_.y = rank;
      } else if (c == 'k') {
        if (!black_king_position_.IsInvalid()) {
          throw InvalidFENException(fen, "Found two black kings");
        }
        black_king_position_.x = file;
        black_king_position_.y = rank;
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

void Board::SetKingPosition(bool white, size_t x, size_t y) {
  if (white) {
    white_king_position_.x = x;
    white_king_position_.y = y;
  } else {
    black_king_position_.x = x;
    black_king_position_.y = y;
  }
}

bool Board::IsKingInCheckHelper(const Square& starting_square, bool white, int x_offset, int y_offset) const {
  const bool is_diagonal = x_offset != 0 && y_offset != 0;
  int x = static_cast<int>(starting_square.x);
  int y = static_cast<int>(starting_square.y);
  bool first_iteration = true;
  while (1) {
    x += x_offset;
    y += y_offset;
    if (x < 0 || x > 7 || y < 0 || y > 7) {
      return false;
    }
    char figure = squares_[x][y];
    if (figure == '\0') {
      first_iteration = false;
      continue;
    }
    if (white) {
      if (first_iteration && figure == 'k') {
        return true;
      }
      if (is_diagonal) {
        return figure == 'q' || figure == 'b';
      } else {
        return figure == 'q' || figure == 'r';
      }
    } else {
      if (first_iteration && figure == 'K') {
        return true;
      }
      if (is_diagonal) {
        return figure == 'Q' || figure == 'B';
      } else {
        return figure == 'Q' || figure == 'R';
      }
    }
    first_iteration = false;
  }
  return false;
}

bool Board::IsKingInCheck(bool white) const {
  const Square& starting_square = white ? white_king_position_ : black_king_position_;
  // First check straight lines.
  if (IsKingInCheckHelper(starting_square, white, 0, 1)) return true;
  if (IsKingInCheckHelper(starting_square, white, 1, 0)) return true;
  if (IsKingInCheckHelper(starting_square, white, 0, -1)) return true;
  if (IsKingInCheckHelper(starting_square, white, -1, 0)) return true;
  // Now check diagonals.
  if (IsKingInCheckHelper(starting_square, white, 1, 1)) return true;
  if (IsKingInCheckHelper(starting_square, white, 1, -1)) return true;
  if (IsKingInCheckHelper(starting_square, white, -1, 1)) return true;
  if (IsKingInCheckHelper(starting_square, white, -1, -1)) return true;
  // Check for checks by knight.
  int x = starting_square.x;
  int y = starting_square.y;
  char knight = white ? 'n' : 'N';
  if (IsFigureAtGivenCoordinates(x + 2, y + 1, knight)) return true;
  if (IsFigureAtGivenCoordinates(x + 2, y - 1, knight)) return true;
  if (IsFigureAtGivenCoordinates(x + 1, y - 2, knight)) return true;
  if (IsFigureAtGivenCoordinates(x - 1, y - 2, knight)) return true;
  if (IsFigureAtGivenCoordinates(x - 2, y + 1, knight)) return true;
  if (IsFigureAtGivenCoordinates(x - 2, y - 1, knight)) return true;
  if (IsFigureAtGivenCoordinates(x + 1, y + 2, knight)) return true;
  if (IsFigureAtGivenCoordinates(x - 1, y + 2, knight)) return true;
  // Now it's time for pawns.
  char pawn = white ? 'p' : 'P';
  int offset = white ? 1 : -1;
  if (IsFigureAtGivenCoordinates(x + 1, y + offset, pawn)) return true;
  if (IsFigureAtGivenCoordinates(x - 1, y + offset, pawn)) return true;
  return false;
}

bool Board::IsFigureAtGivenCoordinates(int x, int y, char figure) const {
  return x >= 0 && x <= 7 && y >= 0 && y <=7 && squares_[x][y] == figure;
}

char Board::at(const char* square) const {
  return squares_[square[0] - 'a'][square[1] - '1'];
}

Square Board::KingPosition(bool white) const {
  return white ? white_king_position_ : black_king_position_;
}
