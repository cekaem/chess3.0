#ifndef MOVE_CALCULATOR_H_
#define MOVE_CALCULATOR_H_

#include <iostream>
#include <utility>
#include <vector>

#include "Board.h"

struct Move {
  Square old_square;
  Square new_square;
  char promotion_to = 0x0;
};

struct SerializedMove {
  SerializedMove(size_t old_x, size_t old_y, size_t new_x, size_t new_y, char promotion);
  Move ToMove() const;

  unsigned short data{0};
};

std::ostream& operator<<(std::ostream& ostr, const SerializedMove& move_serialized);
std::ostream& operator<<(std::ostream& ostr, const Move& move);

struct InvalidMoveException {
  InvalidMoveException(const SerializedMove& m) : move(m) {}

  const SerializedMove move;
};

class MoveCalculator {
 public:
  std::vector<SerializedMove> CalculateAllMoves(Board& board);
  std::vector<SerializedMove> CalculateAllMoves(const std::string& fen);
  static char ApplyMoveOnBoard(Board& board, const SerializedMove& serialized_move);
  static void RevertMoveOnBoard(Board& board,
                                const SerializedMove& serialized_move,
                                char captured_figure);

 private:
  void HandlePawnMoves(size_t x, size_t y);
  void HandleKnightMoves(size_t x, size_t y);
  void HandleBishopMoves(size_t x, size_t y);
  void HandleRookMoves(size_t x, size_t y);
  void HandleQueenMoves(size_t x, size_t y);
  void HandleKingMoves(size_t x, size_t y);
  void HandleCastlings(size_t x, size_t y);
  bool CanCastle(bool white_king, bool king_side) const;
  void AddCastling(bool white_king, bool king_side);
  void CheckNewSquareAndMaybeAddMove(size_t old_x, size_t old_y, int new_x, int new_y);
  void MaybeAddMove(size_t old_x, size_t old_y, size_t new_x, size_t new_y, bool promotion = false);
  void HandleMovesHelper(size_t x, size_t y, int x_offset, int y_offset);

  static void UpdateCastlings(Board& copy, char figure, size_t old_x, size_t old_y);
  static void UpdateEnPassantTargetSquare(
      Board& copy,
      char figure,
      size_t old_x,
      size_t old_y,
      size_t new_y);
  static Castling IsMoveCastling(
      const Board& board,
      const Move& serialized_move);
  static void MaybeUpdateRookPositionAfterCastling(
      Board& board,
      const Move& serialized_move);

  Board* board_{nullptr};
  std::vector<SerializedMove> moves_;
};

#endif  // MOVE_CALCULATOR_H_
