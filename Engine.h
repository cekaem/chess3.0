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
  Move CalculateBestMove(Board& board);
  unsigned NodesCalculated() const { return nodes_calculated_; }

 private:
  struct EngineMove {
    EngineMove() {}
    EngineMove(const SerializedMove& move);
    ~EngineMove();

    SerializedMove move;
    unsigned short number_of_children{0u};
    EngineMove* children{nullptr};
  };

  struct BorderValues {
    static const int NOT_SET = 999;
    bool is_zero{false};
    int the_biggest_value{NOT_SET};
    int the_lowest_value {NOT_SET};
    int the_biggest_negative_value{NOT_SET};
    int the_lowest_positive_value{NOT_SET};
  };

  EngineMove* GenerateEngineMovesForBoard(Board& board, short& number_of_children);
  void GenerateNextDepth(const Board& board, EngineMove* moves, short number_of_children);
  BorderValues GetBorderValuesForVector(const std::vector<short>& vec) const;
  short CalculateMovesToMate(std::vector<short> moves_to_mate, bool white_to_move) const;
  void UpdateMoveEvalBasedOnChildren(EngineMove& move) const;
  short EvaluateChildren(Board& board,
                         EngineMove* moves,
                         short number_of_children,
                         bool remember_best_move) const;

  unsigned max_depth_{0u};
  unsigned max_time_{0u};
  bool playing_white_;
  bool continue_calculations_;
  unsigned nodes_calculated_;
  mutable Move best_move_;
};

#endif  // ENGINE_H
