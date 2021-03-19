#ifndef MOVE_CALCULATOR_H_
#define MOVE_CALCULATOR_H_

#include <iostream>
#include <vector>

#include "Board.h"

struct Move {
  Move(Board&& b, size_t ox, size_t oy, size_t nx, size_t ny, char p, bool fc) :
    board(std::move(b)), old_x(ox), old_y(oy), new_x(nx), new_y(ny), promotion_to(p), figure_captured(fc) {}
  Move(Board& b, size_t ox, size_t oy, size_t nx, size_t ny, char p, bool fc) :
    board(b), old_x(ox), old_y(oy), new_x(nx), new_y(ny), promotion_to(p), figure_captured(fc) {}

  const Board board;
  const size_t old_x;
  const size_t old_y;
  const size_t new_x;
  const size_t new_y;
  const char promotion_to{0x0};
  const bool figure_captured{false};
};

std::ostream& operator<<(std::ostream& os, const Move& move);

class MoveCalculator {
 public:
  std::vector<Move> CalculateAllMoves(const Board& board);
  std::vector<Move> CalculateAllMoves(const std::string& fen);

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
  void UpdateCastlings(Board& copy, char figure, size_t old_x, size_t old_y) const;
  void UpdateEnPassantTargetSquare(Board& copy, char figure, size_t old_x, size_t old_y, size_t new_y) const;

  const Board* board_{nullptr};
  std::vector<Move> moves_;
};

#endif  // MOVE_CALCULATOR_H_
