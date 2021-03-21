#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

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
  Move CalculateBestMove(const Board& board);

 private:
  struct EngineMove {
    EngineMove(const Board& move);
    Board board;
    double eval;
    int mate_in{0};
    std::vector<EngineMove> children;
  };

  using EngineMoves = std::vector<EngineMove>;

  EngineMoves GenerateEngineMovesForBoard(const Board& board) const;
  Move FindMoveForBoard(const Board& initial_board, const Board& dest_board) const;
  void LookForBestMove(EngineMoves& moves);
  double FindBestEval(const EngineMoves& moves) const;
  EngineMoves FindMovesWithEvalInRoot(double eval) const;
  int CheckForMate(const EngineMoves& moves) const;
  EngineMoves FindMovesWithMateInInRoot(double mate_in) const;

  EngineMoves root_;
  bool playing_white_;
};

#endif  // ENGINE_H
