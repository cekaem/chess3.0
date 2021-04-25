#include "MoveCalculator.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>


std::ostream& operator<<(std::ostream& os, const Move& move) {
  os << static_cast<char>(move.old_square.x + 'a') << static_cast<char>(move.old_square.y + '1') << "-";
  os << static_cast<char>(move.new_square.x + 'a') << static_cast<char>(move.new_square.y + '1');
  return os;
}

std::ostream& operator<<(std::ostream& os, const SerializedMove& move_serialized) {
  Move move = move_serialized.ToMove();
  os << move;
  return os;
}

SerializedMove::SerializedMove(size_t old_x, size_t old_y,
                               size_t new_x, size_t new_y,
                               char promotion) {
  data = old_x;
  data <<= 3;
  data |= old_y;
  data <<= 3;
  data |= new_x;
  data <<= 3;
  data |= new_y;
  data <<= 3;
  if (promotion) {
    data |= 4;
    switch (promotion) {
      case 'N':
      case 'n':
        break;
      case 'B':
      case 'b':
        data |= 1;
        break;
      case 'R':
      case 'r':
        data |= 2;
        break;
      case 'Q':
      case 'q':
        data |= 3;
        break;
      default:
        assert(!"Uknown promotion figure.");
        break;
    }
  }
  data <<= 1;
}

Move SerializedMove::ToMove() const {
  Move result;
  result.old_square.x = (data >> 13);
  result.old_square.y = (data >> 10) & 7;
  result.new_square.x = (data >> 7) & 7;
  result.new_square.y = (data >> 4) & 7;
  bool promotion = (data >> 3) & 1;
  if (promotion) {
    char promotion_to = (data >> 1) & 3;
    switch (promotion_to) {
      case 0:
        result.promotion_to = 'N';
        break;
      case 1:
        result.promotion_to = 'B';
        break;
      case 2:
        result.promotion_to = 'R';
        break;
      case 3:
        result.promotion_to = 'Q';
        break;
      default:
        assert(!"Unknown promotion_to");
        break;
    }
  }
  return result;
}

std::vector<SerializedMove> MoveCalculator::CalculateAllMoves(const std::string& fen) {
  Board board(fen);
  return CalculateAllMoves(board);
}

std::vector<SerializedMove> MoveCalculator::CalculateAllMoves(Board& board) {
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

void MoveCalculator::UpdateCastlings(Board& board, char figure, size_t old_x, size_t old_y) {
  if (figure == 'K') {
    board.UnsetCanCastle(Castling::K);
    board.UnsetCanCastle(Castling::Q);
  } else if (figure == 'k') {
    board.UnsetCanCastle(Castling::k);
    board.UnsetCanCastle(Castling::q);
  } else if (figure == 'R') {
    if (old_x == 0u && old_y == 0u) {
      board.UnsetCanCastle(Castling::Q);
    } else if (old_x == 7u && old_y == 0u) {
      board.UnsetCanCastle(Castling::K);
    }
  } else if (figure == 'r') {
    if (old_x == 0u && old_y == 7u) {
      board.UnsetCanCastle(Castling::q);
    } else if (old_x == 7u && old_y == 7u) {
      board.UnsetCanCastle(Castling::k);
    }
  }
}

void MoveCalculator::UpdateEnPassantTargetSquare(Board& board, char figure, size_t old_x, size_t old_y, size_t new_y) {
  if (figure == 'P' && old_y == 1u && new_y == 3u) {
    board.SetEnPassantTargetSquare(Square(old_x, 2u));
  } else if (figure == 'p' && old_y == 6u && new_y == 4u) {
    board.SetEnPassantTargetSquare(Square(old_x, 5u));
  } else {
    board.InvalidateEnPassantTargetSquare();
  }
}

Castling MoveCalculator::IsMoveCastling(const Board& board,
                                        const Move& move) {
  if (abs(move.old_square.x - move.new_square.x) != 2u) {
    return Castling::LAST;
  }
  if (move.new_square == Square(6, 0)) {
    return Castling::K;
  } else if (move.new_square == Square(2, 0)) {
    return Castling::Q;
  } else if (move.new_square == Square(6, 7)) {
    return Castling::k;
  } else if (move.new_square == Square(2, 7)) {
    return Castling::q;
  }
  assert(!"Unexpected new square for king");
  return Castling::LAST;
}

void MoveCalculator::MaybeUpdateRookPositionAfterCastling(
    Board& board,
    const Move& move) {
  switch (IsMoveCastling(board, move)) {
    case Castling::K:
      board.at(7u, 0u) = 0x0;
      board.at(5u, 0u) = 'R';
      break;
    case Castling::Q:
      board.at(0u, 0u) = 0x0;
      board.at(3u, 0u) = 'R';
      break;
    case Castling::k:
      board.at(7u, 7u) = 0x0;
      board.at(5u, 7u) = 'r';
      break;
    case Castling::q:
      board.at(0u, 7u) = 0x0;
      board.at(3u, 7u) = 'r';
      break;
    case Castling::LAST:
      break;
  }
}

void MoveCalculator::ApplyMoveOnBoard(Board& board, const SerializedMove& serialized_move) {
  Move move = serialized_move.ToMove();
  const char figure = board.at(move.old_square.x, move.old_square.y);
  char captured_figure = board.at(move.new_square.x, move.new_square.y);
#ifdef _DEBUG_
  if ((captured_figure && !!isupper(captured_figure) == board.WhiteToMove()) ||
      captured_figure == 'K' || captured_figure == 'k' || !figure) {
    throw InvalidMoveException(serialized_move);
  }
#endif
  const bool white_to_move = board.WhiteToMove();
  const Square en_passant_target_square = board.EnPassantTargetSquare();
  const bool en_passant_capture =
      en_passant_target_square.x == move.new_square.x &&
      en_passant_target_square.y == move.new_square.y &&
      figure == (white_to_move ? 'P' : 'p');
  UpdateFiguresPositionsOnBoard(board, move);
  board.at(move.old_square.x, move.old_square.y) = 0x0;
  board.at(move.new_square.x, move.new_square.y) = figure;
  if (en_passant_capture) {
    captured_figure = 'E';
    const size_t captured_pawn_y = white_to_move ? 4u : 3u;
    board.at(en_passant_target_square.x, captured_pawn_y) = 0x0;
  }
  if (figure == 'K' || figure == 'k') {
    MaybeUpdateRookPositionAfterCastling(board, move);
  }
#ifdef _DEBUG_
  if (board.IsKingInCheck(white_to_move)) {
    throw InvalidMoveException(serialized_move);
  }
#endif
  board.ChangeSideToMove();
  if (!white_to_move) {
    board.IncrementFullMoveNumber();
  }
  if (captured_figure || figure == 'P' || figure == 'p') {
    board.ResetHalfMoveClock();
  } else {
    board.IncrementHalfMoveClock();
  }
  UpdateCastlings(board, figure, move.old_square.x, move.old_square.y);
  UpdateEnPassantTargetSquare(board, figure, move.old_square.x, move.old_square.y, move.new_square.y);
  if (move.promotion_to) {
    board.at(move.new_square.x, move.new_square.y) =
      white_to_move ? move.promotion_to : tolower(move.promotion_to);
  }
}

void MoveCalculator::UpdateFiguresPositionsOnBoard(Board& board, const Move& move) {
  switch (board.at(move.old_square)) {
    case 'P':
    case 'p':
    case 'N':
    case 'n':
      break;
    case 'Q':
    case 'q':
    case 'K':
    case 'k':
    case 'R':
    case 'r':
    case 'B':
    case 'b': {
      Figure figure = static_cast<Figure>(FigureCharToInt(board.at(move.old_square)));
      auto& vec = board.FiguresPositions(figure);
      auto iter = std::find(vec.begin(), vec.end(), move.old_square);
      assert(iter != vec.end());
      *iter = move.new_square;
      break;
    }
    default:
      assert(!"Unexpected char");
  }

  switch (board.at(move.new_square)) {
    case 0x0:
    case 'P':
    case 'p':
      break;
    case 'Q':
    case 'q':
    case 'R':
    case 'r':
    case 'B':
    case 'b': {
      Figure figure = static_cast<Figure>(FigureCharToInt(board.at(move.new_square)));
      auto& vec = board.FiguresPositions(figure);
      vec.erase(std::remove(vec.begin(), vec.end(), move.new_square), vec.end());
      break;
    }
    case 'N':
      board.DecrementNumberOfKnights(true);
      break;
    case 'n':
      board.DecrementNumberOfKnights(false);
      break;
    case 'K':
    case 'k':
      assert(!"King can't be captured");
      break;
    default:
      assert(!"Unexpected char");
  }

  char promotion_to = move.promotion_to;
  if (!board.WhiteToMove()) {
    promotion_to = tolower(promotion_to);
  }
  switch (promotion_to) {
    case 0x0:
      break;
    case 'Q':
    case 'q':
    case 'R':
    case 'r':
    case 'B':
    case 'b': {
      Figure figure = static_cast<Figure>(FigureCharToInt(promotion_to));
      auto& vec = board.FiguresPositions(figure);
      vec.push_back(move.new_square);
      break;
    }
    case 'N':
      board.IncrementNumberOfKnights(true);
      break;
    case 'n':
      board.IncrementNumberOfKnights(false);
      break;
    default:
      assert(!"Unexpected char");
      break;
  }
}

void MoveCalculator::MaybeAddMove(size_t old_x, size_t old_y, size_t new_x, size_t new_y, bool promotion) {
  const char figure = board_->at(old_x, old_y);
  char captured_figure = board_->at(new_x, new_y);
  if (captured_figure && !!isupper(captured_figure) == board_->WhiteToMove()) {
    return;
  }
  board_->at(new_x, new_y) = figure;
  board_->at(old_x, old_y) = 0x0;
  const Square en_passant_target_square = board_->EnPassantTargetSquare();
  const bool white_to_move = board_->WhiteToMove();
  const bool en_passant_capture =
      en_passant_target_square.x == new_x &&
      en_passant_target_square.y == new_y &&
      figure == (white_to_move ? 'P' : 'p');
  size_t captured_pawn_y = 0;
  if (en_passant_capture) {
    captured_figure = white_to_move ? 'p' : 'P';
    captured_pawn_y = white_to_move ? 4u : 3u;
    board_->at(en_passant_target_square.x, captured_pawn_y) = 0x0;
  }
  if (figure == 'K' || figure == 'k') {
    board_->SetKingPosition(white_to_move, new_x, new_y);
  }
  const bool move_is_valid = !board_->IsKingInCheck(white_to_move);
  if (en_passant_capture) {
    board_->at(en_passant_target_square.x, captured_pawn_y) = captured_figure;
    board_->at(new_x, new_y) = 0x0;
  } else {
    board_->at(new_x, new_y) = captured_figure;
  }
  board_->at(old_x, old_y) = figure;
  if (figure == 'K' || figure == 'k') {
    board_->SetKingPosition(white_to_move, old_x, old_y);
  }

  if (!move_is_valid) {
    return;
  }

  if (promotion) {
    moves_.push_back({old_x, old_y, new_x, new_y, white_to_move ? 'B' : 'b'});
    moves_.push_back({old_x, old_y, new_x, new_y, white_to_move ? 'N' : 'n'});
    moves_.push_back({old_x, old_y, new_x, new_y, white_to_move ? 'R' : 'r'});
    moves_.push_back({old_x, old_y, new_x, new_y, white_to_move ? 'Q' : 'q'});
  } else {
    moves_.push_back({old_x, old_y, new_x, new_y, 0x0});
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
  const size_t rank = white_king ? 0u : 7u;
  const size_t old_x = 4u;
  const size_t new_x = king_side ? 6u : 2u;
  moves_.push_back({old_x, rank, new_x, rank, 0x0});
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
  if (!king_side) {
    const size_t third_x = second_x + offset;
    if (board_->at(third_x, rank)) {
      return false;
    }
  }
  if (board_->IsKingInCheck(white_king)) {
    return false;
  }
  const char king = white_king ? 'K' : 'k';
  assert(board_->at(king_staring_x, rank) == king);
  board_->at(king_staring_x, rank) = '\0';
  board_->at(first_x, rank) = king;
  board_->SetKingPosition(white_king, first_x, rank);
  bool result = !board_->IsKingInCheck(white_king);
  board_->at(first_x, rank) = 0x0;
  if (result) {
    board_->at(second_x, rank) = king;
    board_->SetKingPosition(white_king, second_x, rank);
    result = !board_->IsKingInCheck(white_king);
    board_->at(second_x, rank) = 0x0;
  }
  board_->at(king_staring_x, rank) = king;
  board_->SetKingPosition(white_king, king_staring_x, rank);
  return result;
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
