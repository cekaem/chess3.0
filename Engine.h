#ifndef ENGINE_H
#define ENGINE_H

#include "Board.h"
#include "MoveCalculator.h"

struct NoMovesException {
  NoMovesException(bool mate) : is_mate(mate) {}
  const bool is_mate;
};

class Engine {
 public:
  Engine();
  Move CalculateBestMove(const Board& board) const;
};

#endif  // ENGINE_H
