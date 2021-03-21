#ifndef ENGINE_H
#define ENGINE_H

#include "Board.h"
#include "MoveCalculator.h"
#include "Types.h"


struct NoMovesException {
  NoMovesException(GameResult r) : result(r) {}
  const GameResult result;
};

class Engine {
 public:
  Engine();
  Move CalculateBestMove(const Board& board) const;
};

#endif  // ENGINE_H
