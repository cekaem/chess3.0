#include "PGNCreator.h"

#include <utility>
#include <vector>


void PGNCreator::GameFinished(PGNCreator::Result result) {
  result_ = result;
}

std::string PGNCreator::GetPGN() const {
  return "";
}

std::vector<std::pair<size_t, size_t>> FindAllMatchingMoves(
    const Board& board,
    const char figure,
    const size_t end_x,
    const size_t end_y) {
  std::vector<std::pair<size_t, size_t>> matching_moves;
  /*
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  std::vector<std::pair<size_t, size_t>> matching_moves;
  for (const auto& move: moves) {
    if (move.
  }
  */
  return matching_moves;
}

std::string PGNCreator::AddMove(const Board& board, const Move& move) {
  /*
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  */
  return "";
}
