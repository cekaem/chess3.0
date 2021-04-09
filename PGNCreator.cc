#include "PGNCreator.h"

#include <cassert>
#include <cctype>
#include <sstream>
#include <vector>

namespace {

enum class MoveTypeFlag {
  NONE = 0x0,
  UNIQUE_DESTINATION = 0x1,
  UNIQUE_SOURCE_FILE = 0x2,
  UNIQUE_SOURCE_RANK = 0x4
};

MoveTypeFlag operator&(MoveTypeFlag flags, MoveTypeFlag flag) {
  return static_cast<MoveTypeFlag>(
      static_cast<int>(flags) & static_cast<int>(flag));
}

MoveTypeFlag operator|=(MoveTypeFlag& flags, MoveTypeFlag flag) {
   flags = static_cast<MoveTypeFlag>(
      static_cast<int>(flags) | static_cast<int>(flag));
   return flags;
}

bool IsUniqueDestination(MoveTypeFlag flag) {
  return (flag & MoveTypeFlag::UNIQUE_DESTINATION) != MoveTypeFlag::NONE;
}

bool IsUniqueSourceFile(MoveTypeFlag flag) {
  return (flag & MoveTypeFlag::UNIQUE_SOURCE_FILE) != MoveTypeFlag::NONE;
}

bool IsUniqueSourceRank(MoveTypeFlag flag) {
  return (flag & MoveTypeFlag::UNIQUE_SOURCE_RANK) != MoveTypeFlag::NONE;
}

char FileNumberToString(size_t file) {
  return static_cast<char>(file + 'a');
}

char RankNumberToString(size_t rank) {
  return static_cast<char>(rank + '1');
}

std::string DestinationSquareToString(const Move& move) {
  std::string result;
  result += FileNumberToString(move.new_square.x);
  result += RankNumberToString(move.new_square.y);
  return result;
}

std::vector<Move> FindAllMatchingMoves(
    const Board& board,
    const Move& move_to_match) {
  std::vector<Move> matching_moves;
  const char figure = board.at(move_to_match.old_square.x, move_to_match.old_square.y);
  assert(figure);
  MoveCalculator calculator;
  Board copy = board.Clone();
  auto moves = calculator.CalculateAllMoves(copy);
  for (const auto& serialized_move: moves) {
    Move move = serialized_move.ToMove();
    if (board.at(move.old_square.x, move.old_square.y) == figure &&
        move.promotion_to == move_to_match.promotion_to &&
        move.new_square.x == move_to_match.new_square.x &&
        move.new_square.y == move_to_match.new_square.y) {
      matching_moves.push_back(move);
    }
  }
  return matching_moves;
}

MoveTypeFlag DetermineMoveType(const Board& board, const Move& move) {
  const char figure = board.at(move.old_square.x, move.old_square.y);
  assert(figure);
  auto matching_moves = FindAllMatchingMoves(board, move);
  assert(!matching_moves.empty());
  MoveTypeFlag flags = MoveTypeFlag::NONE;
  if (matching_moves.size() == 1u) {
     flags |= MoveTypeFlag::UNIQUE_DESTINATION;
  } else {
    size_t number_of_moves_with_matching_file = 0u;
    size_t number_of_moves_with_matching_rank = 0u;
    for (const auto& matching_move: matching_moves) {
      if (matching_move.old_square.x == move.old_square.x) {
        ++number_of_moves_with_matching_file;
      }
      if (matching_move.old_square.y == move.old_square.y) {
        ++number_of_moves_with_matching_rank;
      }
    }
    assert(number_of_moves_with_matching_file);
    assert(number_of_moves_with_matching_rank);
    if (number_of_moves_with_matching_file == 1u) {
      flags |= MoveTypeFlag::UNIQUE_SOURCE_FILE;
    }
    if (number_of_moves_with_matching_rank == 1u) {
      flags |= MoveTypeFlag::UNIQUE_SOURCE_RANK;
    }
  }
  return flags;
}

bool MoveCapturesFigure(const Board& board, const Move& move) {
  if (board.at(move.new_square)) {
    return true;
  }
  const char figure = board.at(move.old_square);
  return (figure == 'P' || figure == 'p') &&
         move.new_square == board.EnPassantTargetSquare();
}

bool IsCastling(char figure, const Move& move, bool king_side) {
  if (figure != 'K' && figure != 'k') {
    return false;
  }
  const size_t expected_rank = (figure == 'K' ? 0u : 7u);
  const size_t expected_source_file = 4u;
  const size_t expected_destination_file = (king_side ? 6u : 2u);
  return move.old_square.x == expected_source_file &&
         move.new_square.x == expected_destination_file &&
         move.new_square.y == expected_rank;
}

std::string MoveToString(const Board& board, const SerializedMove& serialized_move) {
  Move move = serialized_move.ToMove();
  const char figure = board.at(move.old_square);
  assert(figure);
  if (IsCastling(figure, move, true)) {
    return "O-O";
  }
  if (IsCastling(figure, move, false)) {
    return "O-O-O";
  }
  std::string result;
  if (figure != 'P' && figure != 'p') {
    result += toupper(figure);
    MoveTypeFlag flags = DetermineMoveType(board, move);
    if (!IsUniqueDestination(flags)) {
      if (IsUniqueSourceFile(flags)) {
        result += FileNumberToString(move.old_square.x);
      } else if (IsUniqueSourceRank(flags)) {
        result += RankNumberToString(move.old_square.y);
      } else {
        assert(flags == MoveTypeFlag::NONE);
        result += FileNumberToString(move.old_square.x);
        result += RankNumberToString(move.old_square.y);
      }
    }
  }
  if (MoveCapturesFigure(board, move)) {
    if (figure == 'P' || figure == 'p') {
      result += FileNumberToString(move.old_square.x);
    }
    result += 'x';
  }
  result += DestinationSquareToString(move);
  if (move.promotion_to) {
    result += toupper(move.promotion_to);
  }
  Board board_after_move = board.Clone();
  MoveCalculator::ApplyMoveOnBoard(board_after_move, serialized_move);
  bool is_check = false;
  bool is_mate = false;
  if (board_after_move.IsKingInCheck(board_after_move.WhiteToMove())) {
    is_check = true;
  }
  if (is_check) {
    MoveCalculator calculator;
    is_mate = calculator.CalculateAllMoves(board_after_move).empty();
  }
  if (is_mate) {
    result += '#';
  } else if (is_check) {
    result += '+';
  }
  return result;
}

}  // unnamed namespace


void PGNCreator::GameFinished(GameResult result) {
  result_ = result;
}

std::string PGNCreator::GetPGN() const {
  std::stringstream ss;
  std::string result_string;
  switch (result_) {
    case GameResult::WHITE_WON:
      result_string.append("1-0");
      break;
    case GameResult::BLACK_WON:
      result_string.append("0-1");
      break;
    case GameResult::DRAW:
      result_string.append("1/2-1/2");
      break;
    case GameResult::NONE:
      break;
  }
  if (!result_string.empty()) {
    ss << "[Result \"" << result_string << "\"]" << std::endl;
  }
  for (const auto& move: moves_) {
    ss << move << " ";
  }
  if (!result_string.empty()) {
    ss << result_string;
  }
  return ss.str();
}

std::string PGNCreator::AddMove(const Board& board, const SerializedMove& move) {
  std::string move_str = MoveToString(board, move);
  std::stringstream ss;
  if (board.WhiteToMove()) {
    ss << board.FullMoveNumber() << ". ";
  }
  ss << move_str;
  moves_.push_back(ss.str());
  return move_str;
}
