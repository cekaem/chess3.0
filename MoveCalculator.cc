#include "MoveCalculator.h"

#include <cassert>
#include <cctype>

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
          break;
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
          assert(!"Unexpeted char");
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

void MoveCalculator::MaybeAddMove(size_t old_x, size_t old_y, size_t new_x, size_t new_y, bool promotion) {
  char captured_figure = board_->at(new_x, new_y);
  if (captured_figure && !!isupper(captured_figure) == board_->WhiteToMove()) {
    return;
  }
  const char figure = board_->at(old_x, old_y);
  const bool white_to_move = board_->WhiteToMove();
  const Square en_passant_target_square = board_->EnPassantTargetSquare();
  const bool en_passant_capture =
      en_passant_target_square.x == new_x &&
      en_passant_target_square.y == new_y &&
      figure == (white_to_move ? 'P' : 'p');
  Board copy = *board_;
  assert(figure);
  assert(captured_figure != 'K' && captured_figure != 'k');
  copy.at(old_x, old_y) = '\0';
  copy.at(new_x, new_y) = figure;
  if (en_passant_capture) {
    captured_figure = white_to_move ? 'p' : 'P';
    const size_t captured_pawn_y = white_to_move ? 4u : 3u;
    copy.at(en_passant_target_square.x, captured_pawn_y) = 0x0;
  }
  if (figure == 'K') {
    copy.SetKingPosition(true, new_x, new_y);
  } else if (figure == 'k') {
    copy.SetKingPosition(false, new_x, new_y);
  }
  if (copy.IsKingInCheck(white_to_move)) {
    return;
  }
  copy.ChangeSideToMove();
  if (!white_to_move) {
    copy.IncrementFullMoveNumber();
  }
  copy.InvalidateEnPassantTargetSquare();
  if (captured_figure || figure == 'P' || figure == 'p') {
    copy.ResetHalfMoveClock();
  } else {
    copy.IncrementHalfMoveClock();
  }
  if (promotion) {
    auto AddPromotionMove = [this, &copy, old_x, old_y, new_x, new_y, captured_figure](char promoted_to) {
      copy.at(new_x, new_y) = promoted_to;
      moves_.push_back({copy, old_x, old_y, new_x, new_y, promoted_to, !!captured_figure});
    };
    AddPromotionMove(white_to_move ? 'Q' : 'q');
    AddPromotionMove(white_to_move ? 'R' : 'r');
    AddPromotionMove(white_to_move ? 'N' : 'n');
    AddPromotionMove(white_to_move ? 'B' : 'b');
  } else {
    moves_.push_back({std::move(copy), old_x, old_y, new_x, new_y, 0x0, !!captured_figure});
  }
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
    MaybeAddMove(x, y, new_x, new_y);
    if (board_->at(new_x, new_y)) {
      break;
    }
  }
}

void MoveCalculator::HandlePawnMoves(size_t x, size_t y) {
  assert(y != 0u && y != 7u);
  const bool white_move = board_->WhiteToMove();
  const size_t starting_rank = white_move ? 1u : 6u;
  const bool promotion = white_move ? (y == 6u) : (y == 1u);
  const int offset = white_move ? 1 : -1;
  if (!board_->at(x, y + offset)) {
    MaybeAddMove(x, y, x, y + offset, promotion);
    if (y == starting_rank && !board_->at(x, y + 2 * offset)) {
      MaybeAddMove(x, y, x, y + 2 * offset);
    }
  }
  auto HandlePawnCaptureAtSquare = [this, x, y, promotion](size_t new_x, size_t new_y) {
    bool is_en_passant_square = board_->EnPassantTargetSquare().x == new_x &&
                                board_->EnPassantTargetSquare().y == new_y;
    if (board_->at(new_x, new_y) || is_en_passant_square) {
      MaybeAddMove(x, y, new_x, new_y, promotion);
    }
  };
  if (x < 7u) {
    HandlePawnCaptureAtSquare(x + 1u, y + offset);
  }
  if (x > 0u) {
    HandlePawnCaptureAtSquare(x - 1u, y + offset);
  }
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

bool MoveCalculator::CanCastle(bool white_king, bool king_side) const {
  if (white_king) {
    if (!board_->CanCastle(king_side ? Castling::K : Castling::Q)) {
      return false;
    }
  } else {
    if (!board_->CanCastle(king_side ? Castling::k : Castling::q)) {
      return false;
    }
  }
  const size_t rank = white_king ? 0u : 7u;
  const size_t rook_x = king_side ? 7u : 0u;
  if (board_->at(rook_x, rank) != (white_king ? 'R' : 'r')) {
    return false;
  }

  const size_t king_staring_x = 4u;
  const int offset = king_side ? 1 : -1;
  const size_t first_x = king_staring_x + offset;
  const size_t second_x = first_x + offset;
  if (board_->at(first_x, rank) || board_->at(second_x, rank)) {
    return false;
  }
  if (board_->IsKingInCheck(white_king)) {
    return false;
  }
  Board copy = *board_;
  assert(copy.at(king_staring_x, rank) == (white_king ? 'K' : 'k'));
  copy.at(king_staring_x, rank) = '\0';
  copy.at(first_x, rank) = white_king ? 'K' : 'k';
  copy.SetKingPosition(white_king, first_x, rank);
  copy.ChangeSideToMove();
  if (copy.IsKingInCheck(white_king)) {
    return false;
  }
  copy.at(first_x, rank) = '\0';
  copy.at(second_x, rank) = white_king ? 'K' : 'k';
  copy.SetKingPosition(white_king, second_x, rank);
  if (copy.IsKingInCheck(white_king)) {
    return false;
  }
  return true;
}

void MoveCalculator::HandleCastlings(size_t x, size_t y) {
  const bool white_to_move = board_->WhiteToMove();
  if (white_to_move) {
    if (x != 4u || y != 0) {
      return;
    }
  } else {
    if (x != 4u || y != 7) {
      return;
    }
  }
  if (CanCastle(white_to_move, true)) {
    AddCastling(white_to_move, true);
  }
  if (CanCastle(white_to_move, false)) {
    AddCastling(white_to_move, false);
  }
}
