#include "PGNCreator.h"

#include <cassert>
#include <cctype>
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
  result += FileNumberToString(move.new_x);
  result += RankNumberToString(move.new_y);
  return result;
}

std::vector<Move> FindAllMatchingMoves(
    const Board& board,
    const Move& move_to_match) {
  std::vector<Move> matching_moves;
  const char figure = board.at(move_to_match.old_x, move_to_match.old_y);
  assert(figure);
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  for (const auto& move: moves) {
    if (board.at(move.old_x, move.old_y) == figure &&
        move.promotion_to == move_to_match.promotion_to &&
        move.new_x == move_to_match.new_x &&
        move.new_y == move_to_match.new_y) {
      matching_moves.push_back(move);
    }
  }
  return matching_moves;
}

MoveTypeFlag DetermineMoveType(const Board& board, const Move& move) {
  const char figure = board.at(move.old_x, move.old_y);
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
      if (matching_move.old_x == move.old_x) {
        ++number_of_moves_with_matching_file;
      }
      if (matching_move.old_y == move.old_y) {
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

bool IsCastling(char figure, const Move& move, bool king_side) {
  if (figure != 'K' && figure != 'k') {
    return false;
  }
  const size_t expected_rank = (figure == 'K' ? 0u : 7u);
  const size_t expected_destination_file = (king_side ? 6u : 2u);
  return move.new_x == expected_destination_file && move.new_y == expected_rank;
}

std::string MoveToString(const Board& board, const Move& move) {
  const char figure = board.at(move.old_x, move.old_y);
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
        result += FileNumberToString(move.old_x);
      } else if (IsUniqueSourceRank(flags)) {
        result += RankNumberToString(move.old_y);
      } else {
        assert(flags == MoveTypeFlag::NONE);
        result += FileNumberToString(move.old_x);
        result += RankNumberToString(move.old_y);
      }
    }
  }
  if (move.figure_captured) {
    if (figure == 'P' || figure == 'p') {
      result += FileNumberToString(move.old_x);
    }
    result += 'x';
  }
  result += DestinationSquareToString(move);
  if (move.promotion_to) {
    result += toupper(move.promotion_to);
  }
  bool is_check = false;
  bool is_mate = false;
  if (move.board.IsKingInCheck(move.board.WhiteToMove())) {
    is_check = true;
  }
  if (is_check) {
    MoveCalculator calculator;
    is_mate = calculator.CalculateAllMoves(move.board).empty();
  }
  if (is_mate) {
    result += '#';
  } else if (is_check) {
    result += '+';
  }
  return result;
}

}  // unnamed namespace


void PGNCreator::GameFinished(PGNCreator::Result result) {
  result_ = result;
}

std::string PGNCreator::GetPGN() const {
  return "";
}

std::string PGNCreator::AddMove(const Board& board, const Move& move) {
  std::string move_str = MoveToString(board, move);
  moves_.push_back(move_str);
  return move_str;
}
