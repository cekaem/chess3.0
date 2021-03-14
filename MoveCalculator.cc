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
  copy.InvalidateEnPassantTargetSquare();
  if (!bitten_figure) {
    copy.IncrementHalfMoveClock();
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
  CheckNewSquareAndMaybeAddMove(x, y, x + 2, y + 1);
  CheckNewSquareAndMaybeAddMove(x, y, x + 2, y - 1);
  CheckNewSquareAndMaybeAddMove(x, y, x + 1, y - 2);
  CheckNewSquareAndMaybeAddMove(x, y, x - 1, y - 2);
  CheckNewSquareAndMaybeAddMove(x, y, x - 2, y + 1);
  CheckNewSquareAndMaybeAddMove(x, y, x - 2, y - 1);
  CheckNewSquareAndMaybeAddMove(x, y, x + 1, y + 2);
  CheckNewSquareAndMaybeAddMove(x, y, x - 1, y + 2);
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
  HandleCastlings(x, y);
}

void MoveCalculator::AddCastling(bool white_king, bool king_side) {
  Board copy = *board_;
  const size_t rank = white_king ? 0u : 7u;
  const size_t king_old_x = 4u;
  const size_t king_new_x = king_side ? 6u : 2u;
  const size_t rook_old_x = king_side ? 7u : 0u;
  const size_t rook_new_x = king_side ? 5u : 3u;
  copy.at(king_old_x, rank) = 0x0;
  copy.at(king_new_x, rank) = white_king ? 'K' : 'k';
  copy.at(rook_old_x, rank) = 0x0;
  copy.at(rook_new_x, rank) = white_king ? 'R' : 'r';
  copy.SetKingPosition(white_king, king_new_x, rank);
  copy.ChangeSideToMove();
  copy.IncrementHalfMoveClock();
  copy.InvalidateEnPassantTargetSquare();
  if (white_king) {
    copy.UnsetCanCastle(Castling::Q);
    copy.UnsetCanCastle(Castling::K);
  } else {
    copy.UnsetCanCastle(Castling::q);
    copy.UnsetCanCastle(Castling::k);
    copy.IncrementFullMoveNumber();
  }
  moves_.push_back({std::move(copy), king_old_x, rank, king_new_x, rank, '\0', false});
}

void MoveCalculator::HandleCastling(size_t x, size_t y, bool king_side) {
  const bool white_to_move = board_->WhiteToMove();
  if (white_to_move) {
    if (!board_->CanCastle(king_side ? Castling::K : Castling::Q)) {
      return;
    }
  } else {
    if (!board_->CanCastle(king_side ? Castling::k : Castling::q)) {
      return;
    }
  }
  const size_t rank = white_to_move ? 0u : 7u;
  const size_t rook_x = king_side ? 7u : 0u;
  if (board_->at(rook_x, rank) != (white_to_move ? 'R' : 'r')) {
    return;
  }

  auto SquareOK = [this, white_to_move, rank](size_t new_x) -> bool {
    if (board_->at(new_x, rank)) {
      return false;
    }
    Board copy = *board_;
    assert(copy.at(4u, rank) == (white_to_move ? 'K' : 'k'));
    copy.at(4u, rank) = '\0';  // 4 == king's current x
    copy.at(new_x, rank) = white_to_move ? 'K' : 'k';
    copy.SetKingPosition(white_to_move, new_x, rank);
    copy.ChangeSideToMove();
    return !copy.IsKingInCheck(white_to_move);
  };

  int x_offset = king_side ? 1 : -1;
  if (SquareOK(x + x_offset) && SquareOK(x + 2 * x_offset)) {
    AddCastling(white_to_move, king_side);
  }
}

void MoveCalculator::HandleCastlings(size_t x, size_t y) {
  if (board_->WhiteToMove()) {
    if (x != 4u || y != 0) {
      return;
    }
  } else {
    if (x != 4u || y != 7) {
      return;
    }
  }
  HandleCastling(x, y, true);
  HandleCastling(x, y, false);
}
