#include "Board.h"

#include <cassert>
#include <sstream>

#include "utils/Utils.h"

size_t FigureCharToInt(char f) {
  Figure figure = Figure::LAST;
  switch (f) {
    case 'Q':
      figure = Figure::Q;
      break;
    case 'q':
      figure = Figure::q;
      break;
    case 'R':
      figure = Figure::R;
      break;
    case 'r':
      figure = Figure::r;
      break;
    case 'B':
      figure = Figure::B;
      break;
    case 'b':
      figure = Figure::b;
      break;
    case 'K':
      figure = Figure::K;
      break;
    case 'k':
      figure = Figure::k;
      break;
    default:
      assert(!"Unkown char");
      break;
  }
  return static_cast<size_t>(figure);
}

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
  bool result =
      b1.WhiteToMove() == b2.WhiteToMove() &&
      b1.HalfMoveClock() == b2.HalfMoveClock() &&
      b1.FullMoveNumber() == b2.FullMoveNumber() &&
      b1.EnPassantTargetSquare() == b2.EnPassantTargetSquare() &&
      b1.CanCastle(Castling::Q) == b2.CanCastle(Castling::Q) &&
      b1.CanCastle(Castling::K) == b2.CanCastle(Castling::K) &&
      b1.CanCastle(Castling::q) == b2.CanCastle(Castling::q) &&
      b1.CanCastle(Castling::k) == b2.CanCastle(Castling::k) &&
      b1.NumberOfKnights(true) == b2.NumberOfKnights(true) &&
      b1.NumberOfKnights(false) == b2.NumberOfKnights(false);
  if (!result) {
    return false;
  }
  for (size_t i = 0; i < static_cast<size_t>(Figure::LAST); ++i) {
    auto fp1 = b1.FiguresPositions(static_cast<Figure>(i));
    auto fp2 = b2.FiguresPositions(static_cast<Figure>(i));
    if (fp1.size() != fp2.size()) {
      return false;
    }
    for (auto s1: fp1) {
      bool found = false;
      for (auto s2: fp2) {
        if (s1 == s2) {
          found = true;
          break;
        }
      }
      if (!found) {
        return false;
      }
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& ostr, const Board& board) {
  ostr << board.CreateFEN();
  return ostr;
}

Board::Board(const std::string& fen) {
  size_t index = HandleFields(fen);
  index = HandleSideToMove(fen, index);
  index = HandleCastlings(fen, index);
  index = HandleEnPassantTargetSquare(fen, index);
  index = HandleHalfMoveClock(fen, index);
  HandleFullMoveNumber(fen, index);
}

Board Board::Clone() const {
  Board clone;
  clone.squares_ = squares_;
  clone.white_to_move_ = white_to_move_;
  clone.halfmove_clock_ = halfmove_clock_;
  clone.fullmove_number_ = fullmove_number_;
  clone.en_passant_target_square_ = en_passant_target_square_;
  clone.castlings_ = castlings_;
  clone.number_of_white_knights_ = number_of_white_knights_;
  clone.number_of_black_knights_ = number_of_black_knights_;
  clone.figures_positions_ = figures_positions_;
  return clone;
}

void Board::HandleFullMoveNumber(const std::string& fen, size_t index) {
  const std::string& full_move_number_str = fen.substr(index);
  if (!utils::str_2_number(full_move_number_str, fullmove_number_)) {
    throw InvalidFENException(fen, "Invalid full move number");
  }
}

size_t Board::HandleHalfMoveClock(const std::string& fen, size_t index) {
  size_t end_index = fen.find(' ', index);
  if (end_index == std::string::npos) {
    throw InvalidFENException(fen, "Error in half move clock section");
  }
  const std::string& half_move_clock_str = fen.substr(index, end_index - index);
  if (!utils::str_2_number(half_move_clock_str, halfmove_clock_)) {
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
  castlings_ = 0x0;
  for (size_t i = index; i < end_index; ++i) {
    switch (fen[i]) {
      case '-':
        if (end_index != index + 1) {
          throw InvalidFENException(fen, "Error in castlings section");
        }
        break;
      case 'Q':
        castlings_ |= (1 << static_cast<size_t>(Castling::Q));
        break;
      case 'q':
        castlings_ |= (1 << static_cast<size_t>(Castling::q));
        break;
      case 'K':
        castlings_ |= (1 << static_cast<size_t>(Castling::K));
        break;
      case 'k':
        castlings_ |= (1 << static_cast<size_t>(Castling::k));
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
  if (figures_positions_[static_cast<size_t>(Figure::K)].size() != 1u) {
    throw InvalidFENException(fen, "Must have exact one white king.");
  }
  if (figures_positions_[static_cast<size_t>(Figure::k)].size() != 1u) {
    throw InvalidFENException(fen, "Must have exact one black king.");
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
    } else {
      switch (c) {
        case 'Q':
        case 'q':
        case 'R':
        case 'r':
        case 'B':
        case 'b':
        case 'K':
        case 'k':
          figures_positions_[FigureCharToInt(c)].push_back({file, rank});
          break;
        case 'N':
          ++number_of_white_knights_;
          break;
        case 'n':
          ++number_of_black_knights_;
          break;
        case 'P':
        case 'p':
          break;
        default:
          throw InvalidFENException(fen, "Invalid char in piece placement section");
          break;
      }
      squares_[file][rank] = c;
      ++file;
    }
    if (file > 8) {
      break;
    }
  }
  if (file != 8) {
    throw InvalidFENException(fen, "Invalid one subsection of piece placement section");
  }
}

const std::vector<Square>& Board::FiguresPositions(Figure f) const {
  return figures_positions_[static_cast<size_t>(f)];
}

std::vector<Square>& Board::FiguresPositions(Figure f) {
  return figures_positions_[static_cast<size_t>(f)];
}

void Board::IncrementNumberOfKnights(bool white) {
  if (white) {
    ++number_of_white_knights_;
  } else {
    ++number_of_black_knights_;
  }
}

void Board::DecrementNumberOfKnights(bool white) {
  if (white) {
    assert(number_of_white_knights_ > 0);
    --number_of_white_knights_;
  } else {
    assert(number_of_black_knights_ > 0);
    --number_of_black_knights_;
  }
}

unsigned short Board::NumberOfKnights(bool white) const {
  return white ? number_of_white_knights_ : number_of_black_knights_;
}

void Board::SetKingPosition(bool white, size_t x, size_t y) {
  if (white) {
    figures_positions_[static_cast<size_t>(Figure::K)][0].x = x;
    figures_positions_[static_cast<size_t>(Figure::K)][0].y = y;
  } else {
    figures_positions_[static_cast<size_t>(Figure::k)][0].x = x;
    figures_positions_[static_cast<size_t>(Figure::k)][0].y = y;
  }
}

bool Board::IsKingInCheckHelper(const Square& king_square, bool white, int x_offset, int y_offset) const {
  const bool is_diagonal = x_offset != 0 && y_offset != 0;
  int x = static_cast<int>(king_square.x);
  int y = static_cast<int>(king_square.y);
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

std::string Board::RankToFEN(size_t rank) const {
  std::string result;
  size_t blank_squares = 0;
  for (size_t file = 0; file < 8; ++file) {
    if (squares_[file][rank] == 0x0) {
      ++blank_squares;
    } else {
      if (blank_squares) {
        result += static_cast<char>(blank_squares + '0');
        blank_squares = 0;
      }
      result += squares_[file][rank];
    }
  }
  if (blank_squares) {
    result += static_cast<char>(blank_squares + '0');
  }
  if (rank) {
    result += '/';
  }
  else {
    result += ' ';
  }
  return result;
}

std::string Board::CastlingsToFEN() const {
  std::string result;
  bool any_castling_possible = false;
  if (CanCastle(Castling::K)) {
    result += 'K';
    any_castling_possible = true;
  }
  if (CanCastle(Castling::Q)) {
    result += 'Q';
    any_castling_possible = true;
  }
  if (CanCastle(Castling::k)) {
    result += 'k';
    any_castling_possible = true;
  }
  if (CanCastle(Castling::q)) {
    result += 'q';
    any_castling_possible = true;
  }
  if (!any_castling_possible) {
    result += "-";
  }
  result += " ";
  return result;
}

std::string Board::EnPassantTargetSquareToFEN() const {
  std::string result;
  if (en_passant_target_square_.IsInvalid()) {
    result += "-";
  } else {
    result += en_passant_target_square_.x + 'a';
    result += en_passant_target_square_.y + '1';
  }
  result += ' ';
  return result;
}

std::string Board::CreateFEN() const {
  std::string fen;
  for (int y = 7; y >= 0; --y) {
    fen += RankToFEN(static_cast<size_t>(y));
  }
  if (white_to_move_) {
    fen += "w ";
  } else {
    fen += "b ";
  }
  fen += CastlingsToFEN();
  fen += EnPassantTargetSquareToFEN();
  std::stringstream ss;
  ss << halfmove_clock_ << " ";
  ss << fullmove_number_;
  fen += ss.str();
  return fen;
}

/*
bool Board::FigureChecksKing(const Square& figure_squre, bool white_king) const {

}
*/

bool Board::IsKingInCheck(bool white) const {
  const Square& king_square = KingPosition(white);
  // First check straight lines.
  if (IsKingInCheckHelper(king_square, white, 0, 1)) return true;
  if (IsKingInCheckHelper(king_square, white, 1, 0)) return true;
  if (IsKingInCheckHelper(king_square, white, 0, -1)) return true;
  if (IsKingInCheckHelper(king_square, white, -1, 0)) return true;
  // Now check diagonals.
  if (IsKingInCheckHelper(king_square, white, 1, 1)) return true;
  if (IsKingInCheckHelper(king_square, white, 1, -1)) return true;
  if (IsKingInCheckHelper(king_square, white, -1, 1)) return true;
  if (IsKingInCheckHelper(king_square, white, -1, -1)) return true;
  // Check for checks by knight.
  int x = king_square.x;
  int y = king_square.y;
  if ((white && number_of_black_knights_ > 0) ||
      (!white && number_of_white_knights_ > 0)) {
    char knight = white ? 'n' : 'N';
    if (IsFigureAtGivenCoordinates(x + 2, y + 1, knight)) return true;
    if (IsFigureAtGivenCoordinates(x + 2, y - 1, knight)) return true;
    if (IsFigureAtGivenCoordinates(x + 1, y - 2, knight)) return true;
    if (IsFigureAtGivenCoordinates(x - 1, y - 2, knight)) return true;
    if (IsFigureAtGivenCoordinates(x - 2, y + 1, knight)) return true;
    if (IsFigureAtGivenCoordinates(x - 2, y - 1, knight)) return true;
    if (IsFigureAtGivenCoordinates(x + 1, y + 2, knight)) return true;
    if (IsFigureAtGivenCoordinates(x - 1, y + 2, knight)) return true;
  }
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
  return white ?
      figures_positions_[static_cast<size_t>(Figure::K)][0] :
      figures_positions_[static_cast<size_t>(Figure::k)][0];
}
