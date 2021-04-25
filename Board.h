#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <iostream>
#include <string>
#include <vector>


struct InvalidFENException {
  InvalidFENException(const std::string& f, const std::string msg)
    : fen(f), error_message(msg) {}
  const std::string fen;
  const std::string error_message;
};

enum class Castling {
  K, Q, k, q, LAST
};

enum class Figure {
  Q, q, R, r, B, b, K, k, LAST
};

size_t FigureCharToInt(char f);

struct Square {
  static const unsigned char INVALID = '9';

  Square() {}
  Square(size_t _x, size_t _y) : x(_x), y(_y) {}
  Square(const std::string& square);

  void Invalidate();

  bool IsInvalid() const {
    return x == INVALID || y == INVALID;
  }

  bool operator==(const Square& other) const;
  bool operator!=(const Square& other) const;

  unsigned char x{INVALID};
  unsigned char y{INVALID};
};

std::ostream& operator<<(std::ostream& os, const Square& square);

class Board {
 public:
  Board(const std::string& fen);
  Board(const Board& other) = delete;
  Board& operator=(const Board& board) = delete;
  Board Clone() const;
  bool IsKingInCheck(bool white) const;
  char& at(size_t x, size_t y) { return squares_[x][y]; }
  char& at(const Square& square) { return squares_[square.x][square.y]; }
  char at(const Square& square) const { return squares_[square.x][square.y]; }
  char at(size_t x, size_t y) const { return squares_[x][y]; }
  char at(const char* square) const;
  bool CanCastle(Castling c) const { return castlings_ & (1 << static_cast<size_t>(c)); }
  Square EnPassantTargetSquare() const { return en_passant_target_square_; }
  Square KingPosition(bool white) const;
  unsigned HalfMoveClock() const { return halfmove_clock_; }
  unsigned FullMoveNumber() const { return fullmove_number_; }
  bool WhiteToMove() const { return white_to_move_; }
  void ChangeSideToMove() { white_to_move_ = !white_to_move_; }
  void IncrementFullMoveNumber() { ++fullmove_number_; }
  void ResetHalfMoveClock() { halfmove_clock_ = 0u; }
  void IncrementHalfMoveClock() { ++halfmove_clock_; }
  void SetHalfMoveClock(unsigned value) { halfmove_clock_ = value; }
  void UnsetCanCastle(Castling c) { castlings_ &=  ~(1 << static_cast<size_t>(c)); }
  void SetEnPassantTargetSquare(Square s) { en_passant_target_square_ = s; }
  void InvalidateEnPassantTargetSquare() { en_passant_target_square_.Invalidate(); }
  void SetKingPosition(bool white, size_t x, size_t y);
  std::string CreateFEN() const;
  unsigned short NumberOfKnights(bool white) const;
  void IncrementNumberOfKnights(bool white);
  void DecrementNumberOfKnights(bool white);
  const std::vector<Square>& FiguresPositions(Figure f) const;
  std::vector<Square>& FiguresPositions(Figure f);
 
 private:
  Board() {}
  Board(Board&& other) = default;
  size_t HandleFields(const std::string& fen);
  void HandleSingleRank(const std::string& fen, const std::string& rank_str, size_t rank);
  size_t HandleSideToMove(const std::string& fen, size_t index);
  size_t HandleCastlings(const std::string& fen, size_t index);
  size_t HandleEnPassantTargetSquare(const std::string& fen, size_t index);
  size_t HandleHalfMoveClock(const std::string& fen, size_t index);
  void HandleFullMoveNumber(const std::string& fen, size_t index);
  bool IsKingInCheckHelper(const Square& starting_square, bool white, int x_offset, int y_offset) const;
  bool IsFigureAtGivenCoordinates(int x, int y, char figure) const;
  std::string RankToFEN(size_t rank) const;
  std::string CastlingsToFEN() const;
  std::string EnPassantTargetSquareToFEN() const;

  std::array<std::array<char, 8>, 8> squares_;
  bool white_to_move_{true};
  unsigned short halfmove_clock_{0};
  unsigned short fullmove_number_{0};
  Square en_passant_target_square_;
  char castlings_{0x0};
  unsigned short number_of_white_knights_{0};
  unsigned short number_of_black_knights_{0};
  std::array<std::vector<Square>, static_cast<size_t>(Figure::LAST)> figures_positions_;
};

bool operator==(const Board& b1, const Board& b2);
std::ostream& operator<<(std::ostream& ostr, const Board& board);

#endif  // BOARD_H
