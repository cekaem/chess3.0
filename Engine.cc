#include "Engine.h"

#include <cstdlib>
#include <ctime>


namespace {

// Returns random value from range [0, max)
size_t GetRandomNumber(size_t max) {
  return rand() % max;
}

}  // unnamed namespace


Engine::Engine() {
  srand(static_cast<unsigned int>(clock()));
}

Move Engine::CalculateBestMove(const Board& board) const {
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  if (moves.empty()) {
    bool is_mate = board.IsKingInCheck(board.WhiteToMove());
    throw NoMovesException(is_mate);
  }
  size_t index = GetRandomNumber(moves.size());
  return moves[index];
}
