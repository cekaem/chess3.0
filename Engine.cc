#include "Engine.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>

#include "utils/Timer.h"

namespace {

static double PawnValue = 1.0;
static double KnightValue = 3.0;
static double BishopValue = 3.0;
static double RookValue = 5.0;
static double QueenValue = 8.0;

static int BorderValue =  1000;

// Returns random value from range [0, max).
size_t GetRandomNumber(size_t max) {
  return rand() % max;
}

double CalculateFiguresValue(const Board& board) {
  double result = 0.0;
  for (size_t x = 0; x < 8u; ++x) {
    for (size_t y = 0; y < 8u; ++y) {
      switch (board.at(x, y)) {
        case 'Q':
          result += QueenValue;
          break;
        case 'q':
          result -= QueenValue;
          break;
        case 'R':
          result += RookValue;
          break;
        case 'r':
          result -= RookValue;
          break;
        case 'B':
          result += BishopValue;
          break;
        case 'b':
          result -= BishopValue;
          break;
        case 'N':
          result += KnightValue;
          break;
        case 'n':
          result -= KnightValue;
          break;
        case 'P':
          result += PawnValue;
          break;
        case 'p':
          result -= PawnValue;
          break;
        case 'K':
        case 'k':
          break;
        default:
          assert(board.at(x, y) == 0x0);
          break;
      }
    }
  }
  return result;
}

double EvaluateMove(const Board& board) {
  double result = CalculateFiguresValue(board);
  return result;
}

bool IsMate(Board& board) {
  MoveCalculator calculator;
  return calculator.CalculateAllMoves(board).empty() &&
         board.IsKingInCheck(board.WhiteToMove());

}

}  // unnamed namespace


Engine::EngineMove::EngineMove(const SerializedMove& m) : move(m) {}

void Engine::EngineMove::Evaluate(Board& board) {
  eval = EvaluateMove(board);
  mate_in = IsMate(board) ? (board.WhiteToMove() ? -1 : 1) : 0;
}

Engine::Engine(unsigned depth) : max_depth_(depth) {
  srand(static_cast<unsigned int>(clock()));
}

Engine::Engine(unsigned depth, unsigned time)
  : max_depth_(depth),
    max_time_(time)  {
  srand(static_cast<unsigned int>(clock()));
}

Engine::EngineMoves Engine::GenerateEngineMovesForBoard(Board& board) {
  EngineMoves result;
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  for (const auto& move: moves) {
    result.push_back(EngineMove(move));
  }
  nodes_calculated_ += result.size();
  return result;
}

void Engine::GenerateNextDepth(const Board& board, EngineMoves& moves) {
  if (!continue_calculations_) {
    return;
  }
  for (auto& move: moves) {
    Board copy = board.Clone();
    MoveCalculator::ApplyMoveOnBoard(copy, move.move);
    if (move.children.empty()) {
      move.children = GenerateEngineMovesForBoard(copy);
    } else {
      GenerateNextDepth(copy, move.children);
    }
  }
}

double Engine::FindBestEval(const EngineMoves& moves) const {
  double result = playing_white_ ? -1000.0 : 1000.0;
  for (const auto& move: moves) {
    if ((playing_white_ && move.eval > result) ||
        (!playing_white_ && move.eval < result)) {
      result = move.eval;
    }
  }
  return result;
}

Engine::EngineMoves Engine::FindMovesWithEvalInRoot(double eval) const {
  EngineMoves result;
  for (const auto& move: root_) {
    if (move.eval == eval) {
      result.push_back(move);
    }
  }
  return result;
}

int Engine::CheckForMate(const EngineMoves& moves) const {
  int result = playing_white_ ? 1000 : -1000;
  bool mate_found = false;
  for (const auto& move: moves) {
    if (!move.mate_in) {
      continue;
    }
    if ((playing_white_ && move.mate_in > 0 && move.mate_in < result) ||
        (!playing_white_ && move.mate_in < 0 && move.mate_in > result)) {
      result = move.mate_in;
      mate_found = true;
    }
  }
  return mate_found ? result : 0;
}

Engine::EngineMoves Engine::FindMovesWithMateInInRoot(int mate_in) const {
  EngineMoves result;
  for (const auto& move: root_) {
    if (move.mate_in == mate_in) {
      result.push_back(move);
    }
  }
  return result;
}

Engine::BorderValues Engine::GetBorderValuesForChildren(const EngineMove& parent) const {
  int the_lowest_value = BorderValue;
  int the_biggest_negative_value = -BorderValue;
  int the_biggest_value = -BorderValue;
  int the_lowest_positive_value = BorderValue;
  BorderValues border_values;
  for (const EngineMove& child: parent.children) {
    if (child.mate_in == 0) {
      border_values.is_zero = true;
    } else if (child.mate_in < 0) {
      if (child.mate_in < the_lowest_value) {
        the_lowest_value = child.mate_in;
      }
      if (child.mate_in > the_biggest_negative_value) {
        the_biggest_negative_value = child.mate_in;
      }
    } else if (child.mate_in > 0) {
      if (child.mate_in > the_biggest_value) {
        the_biggest_value = child.mate_in;
      }
      if (child.mate_in < the_lowest_positive_value) {
        the_lowest_positive_value = child.mate_in;
      }
    }
  }
  if (the_lowest_value != BorderValue) {
    border_values.the_lowest_value = the_lowest_value;
  }
  if (the_biggest_negative_value != -BorderValue) {
    border_values.the_biggest_negative_value = the_biggest_negative_value;
  }
  if (the_biggest_value != -BorderValue) {
    border_values.the_biggest_value = the_biggest_value;
  }
  if (the_lowest_positive_value != BorderValue) {
    border_values.the_lowest_positive_value = the_lowest_positive_value;
  }
  return border_values;
}

bool Engine::UpdateMoveMovesToMateBasedOnChildren(EngineMove& move, bool white_to_move) const {
  const bool is_my_move = playing_white_ == white_to_move;
  BorderValues border_values = GetBorderValuesForChildren(move);
  int result = 0;
  if ((playing_white_ && is_my_move) ||
      (!playing_white_ && !is_my_move)) {
    if (border_values.the_lowest_positive_value != BorderValues::NOT_SET) {
      result = border_values.the_lowest_positive_value;
    } else if (border_values.is_zero) {
      result = 0;
    } else {
      assert(border_values.the_lowest_value < 0);
      result = border_values.the_lowest_value;
    }
  } else if ((!playing_white_ && is_my_move) ||
             (playing_white_ && !is_my_move)) {
    if (border_values.the_biggest_negative_value != BorderValues::NOT_SET) {
      result = border_values.the_biggest_negative_value;
    } else if (border_values.is_zero) {
      result = 0;
    } else {
      assert(border_values.the_biggest_value > 0);
      result = border_values.the_biggest_value;
    }
  }
  if (result > 0) {
    move.mate_in = result + 1;
  } else if (result < 0) {
    move.mate_in = result - 1;
  }
  return result != 0;
}

void Engine::UpdateMoveEvalBasedOnChildren(EngineMove& move) const {
}

void Engine::EvaluateChildrenAndUpdateParent(const Board& board, EngineMoves& parent) const {
  for (auto& move: parent) {
    Board copy = board.Clone();
    MoveCalculator::ApplyMoveOnBoard(copy, move.move);
    if (move.children.empty()) {
      move.Evaluate(copy);
    } else {
      EvaluateChildrenAndUpdateParent(copy, move.children);
      if (!UpdateMoveMovesToMateBasedOnChildren(move, copy.WhiteToMove())) {
        UpdateMoveEvalBasedOnChildren(move);
      }
    }
  }
}

Move Engine::CalculateBestMove(Board& board) {
  assert(max_depth_ > 0u);
  continue_calculations_ = true;
  nodes_calculated_ = 0u;
  utils::Timer timer;
  if (max_time_) {
    timer.start(max_time_, [this]() {
      continue_calculations_ = false;
    });
  }
  playing_white_ = board.WhiteToMove();
  root_ = GenerateEngineMovesForBoard(board);
  if (root_.empty()) {
    GameResult result = GameResult::DRAW;
    const bool is_mate = board.IsKingInCheck(board.WhiteToMove());
    if (is_mate) {
      result = board.WhiteToMove() ? GameResult::BLACK_WON : GameResult::WHITE_WON;
    }
    throw NoMovesException(result);
  }
  for (size_t i = 0; i < max_depth_ - 1; ++i) {
    GenerateNextDepth(board, root_);
  }
  EvaluateChildrenAndUpdateParent(board, root_);
  EngineMoves best_moves;
  int mate_in = CheckForMate(root_);
  if (mate_in) {
    best_moves = FindMovesWithMateInInRoot(mate_in);
  } else {
    double best_eval = FindBestEval(root_);
    best_moves = FindMovesWithEvalInRoot(best_eval);
  }
  assert(!best_moves.empty());
  size_t index = GetRandomNumber(best_moves.size());
  if (max_time_) {
    timer.stop();
  }
  return best_moves[index].move.ToMove();
}
