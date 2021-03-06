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
  Engine(unsigned max_depth);
  Engine(unsigned max_depth, unsigned max_time_for_move);
  Move CalculateBestMove(const Board& board);
  unsigned NodesCalculated() const { return nodes_calculated_; }

 private:
  struct EngineMove {
    EngineMove(const Board& move);
    void Evaluate();

    Board board;
    double eval{0};
    int mate_in{0};
    std::vector<EngineMove> children;
  };

  struct BorderValues {
    static const int NOT_SET = 999;
    bool is_zero{false};
    int the_biggest_value{NOT_SET};
    int the_lowest_value {NOT_SET};
    int the_biggest_negative_value{NOT_SET};
    int the_lowest_positive_value{NOT_SET};
  };

  using EngineMoves = std::vector<EngineMove>;

  EngineMoves GenerateEngineMovesForBoard(const Board& board);
  Move FindMoveForBoard(const Board& initial_board, const Board& dest_board) const;
  void GenerateNextDepth(EngineMoves& moves);
  double FindBestEval(const EngineMoves& moves) const;
  EngineMoves FindMovesWithEvalInRoot(double eval) const;
  int CheckForMate(const EngineMoves& moves) const;
  EngineMoves FindMovesWithMateInInRoot(int mate_in) const;
  BorderValues GetBorderValuesForChildren(const EngineMove& parent) const;
  void EvaluateChildrenAndUpdateParent(EngineMoves& parent) const;
  bool UpdateMoveMovesToMateBasedOnChildren(EngineMove& move) const;
  void UpdateMoveEvalBasedOnChildren(EngineMove& move) const;

  unsigned max_depth_{0u};
  unsigned max_time_{0u};
  EngineMoves root_;
  bool playing_white_;
  bool continue_calculations_;
  unsigned nodes_calculated_;
};

#endif  // ENGINE_H
