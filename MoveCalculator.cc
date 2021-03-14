#include "MoveCalculator.h"

#include <cassert>
#include <cctype>
#include <stdexcept>


std::vector<Move> MoveCalculator::CalculateAllMoves(const Board& board) {
  board_ = &board;
  moves_.clear();
  const bool white_to_move = board.WhiteToMove();
  for (size_t x = 0; x < 8; ++x) {
    for (size_t y = 0; y < 8; ++y) {
      const char c = board.at(x, y);
      if (!c || !!isupper(c) != white_to_move) {
        continue;
      }
      switch (c) {
        case 'P':
        case 'p':
          HandlePawnMoves(x, y);
        case 'B':
        case 'b':
          HandleBishopMoves(x, y);
          break;
        case 'N':
        case 'n':
          HandleKnightMoves(x, y);
          break;
        case 'R':
        case 'r':
          HandleRookMoves(x, y);
          break;
        case 'Q':
        case 'q':
          HandleQueenMoves(x, y);
          break;
        case 'K':
        case 'k':
          HandleKingMoves(x, y);
          break;
        default:
          throw std::runtime_error("MoveCalculator::CalculateAllMoves: unexpected char");
          break;
      }
    }
  }
  return moves_;
}

void MoveCalculator::CheckNewSquareAndMaybeAddMove(size_t old_x, size_t old_y, int new_x, int new_y) {
  if (new_x >= 0 && new_x <= 7 && new_y >= 0 && new_y <= 7) {
    MaybeAddMove(old_x, old_y, new_x, new_y);
  }
}

void MoveCalculator::MaybeAddMove(size_t old_x, size_t old_y, size_t new_x, size_t new_y) {
  Board copy = *board_;
  const char figure = copy.at(old_x, old_y);
  const char bitten_figure = copy.at(new_x, new_y);
  assert(figure);
  assert(bitten_figure != 'K' && bitten_figure != 'k');
  copy.at(old_x, old_y) = '\0';
  copy.at(new_x, new_y) = figure;
  if (figure == 'K') {
    copy.SetKingPosition(true, new_x, new_y);
  } else if (figure == 'k') {
    copy.SetKingPosition(false, new_x, new_y);
  }
  if (copy.IsKingInCheck(copy.WhiteToMove())) {
    return;
  }
  copy.ChangeSideToMove();
  if (copy.WhiteToMove()) {
    copy.IncrementFullMoveNumber();
  }
  moves_.push_back({std::move(copy), old_x, old_y, new_x, new_y, '\0', !!bitten_figure});
}

void MoveCalculator::HandleMovesHelper(size_t x, size_t y, int x_offset, int y_offset) {
  int new_x = x;
  int new_y = y;
  while (1) {
    new_x += x_offset;
    new_y += y_offset;
    if (new_x < 0 || new_x > 7 || new_y < 0 || new_y > 7) {
      break;
    }
    if (!board_->at(new_x, new_y)) {
      MaybeAddMove(x, y, new_x, new_y);
    } else {
      if (!!isupper(board_->at(new_x, new_y) != board_->WhiteToMove())) {
        MaybeAddMove(x, y, new_x, new_y);
      }
      break;
    }
  }
}

void MoveCalculator::HandlePawnMoves(size_t x, size_t y) {
}

void MoveCalculator::HandleKnightMoves(size_t x, size_t y) {
}

void MoveCalculator::HandleBishopMoves(size_t x, size_t y) {
  HandleMovesHelper(x, y, 1, 1);
  HandleMovesHelper(x, y, 1, -1);
  HandleMovesHelper(x, y, -1, 1);
  HandleMovesHelper(x, y, -1, -1);
}

void MoveCalculator::HandleRookMoves(size_t x, size_t y) {
  HandleMovesHelper(x, y, 1, 0);
  HandleMovesHelper(x, y, 0, 1);
  HandleMovesHelper(x, y, -1, 0);
  HandleMovesHelper(x, y, 0, -1);
}

void MoveCalculator::HandleQueenMoves(size_t x, size_t y) {
  HandleBishopMoves(x, y);
  HandleRookMoves(x, y);
}

void MoveCalculator::HandleKingMoves(size_t x, size_t y) {
  CheckNewSquareAndMaybeAddMove(x, y, x + 1, y);
  CheckNewSquareAndMaybeAddMove(x, y, x + 1, y - 1);
  CheckNewSquareAndMaybeAddMove(x, y, x, y - 1);
  CheckNewSquareAndMaybeAddMove(x, y, x - 1, y - 1);
  CheckNewSquareAndMaybeAddMove(x, y, x - 1, y);
  CheckNewSquareAndMaybeAddMove(x, y, x - 1, y + 1);
  CheckNewSquareAndMaybeAddMove(x, y, x, y + 1);
  CheckNewSquareAndMaybeAddMove(x, y, x + 1, y + 1);
  // TODO: handle castlings
}
