#include "Engine.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <memory>

#include "utils/Timer.h"

#include <iostream>

namespace {

/*
static double PawnValue = 1.0;
static double KnightValue = 3.0;
static double BishopValue = 3.0;
static double RookValue = 5.0;
static double QueenValue = 8.0;
*/

static int BorderValue =  1000;

/*
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
*/

bool IsMate(Board& board) {
  MoveCalculator calculator;
  return calculator.CalculateAllMoves(board).empty() &&
         board.IsKingInCheck(board.WhiteToMove());

}

}  // unnamed namespace


Engine::EngineMove::EngineMove(const SerializedMove& m) : move(m) {}

Engine::EngineMove::~EngineMove() {
  delete[] children;
}

Engine::Engine(unsigned depth) : max_depth_(depth) {
  srand(static_cast<unsigned int>(clock()));
}

Engine::Engine(unsigned depth, unsigned time)
  : max_depth_(depth),
    max_time_(time)  {
  srand(static_cast<unsigned int>(clock()));
}

Engine::EngineMove* Engine::GenerateEngineMovesForBoard(Board& board, short& size) {
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  size = moves.size();
  if (size == 0) {
    return nullptr;
  }
  EngineMove* result = new EngineMove[size];
  for (short i = 0; i < size; ++i) {
    result[i].move.data = moves[i].data;
  }
  nodes_calculated_ += size;
  return result;
}

void Engine::GenerateNextDepth(const Board& board, EngineMove* moves, short number_of_children) {
  if (!continue_calculations_) {
    return;
  }
  for (short i = 0; i < number_of_children;  ++i) {
    EngineMove& move = moves[i];
    Board copy = board.Clone();
    MoveCalculator::ApplyMoveOnBoard(copy, move.move);
    if (move.number_of_children == 0) {
      short size;
      move.children = GenerateEngineMovesForBoard(copy, size);
      move.number_of_children = size;
    } else {
      GenerateNextDepth(copy, move.children, move.number_of_children);
    }
  }
}

Engine::BorderValues Engine::GetBorderValuesForVector(
    const std::vector<short>& vec) const {
  int the_lowest_value = BorderValue;
  int the_biggest_negative_value = -BorderValue;
  int the_biggest_value = -BorderValue;
  int the_lowest_positive_value = BorderValue;
  BorderValues border_values;
  for (short mate_in: vec) {
    if (mate_in == 0) {
      border_values.is_zero = true;
    } else if (mate_in < 0) {
      if (mate_in < the_lowest_value) {
        the_lowest_value = mate_in;
      }
      if (mate_in > the_biggest_negative_value) {
        the_biggest_negative_value = mate_in;
      }
    } else if (mate_in > 0) {
      if (mate_in > the_biggest_value) {
        the_biggest_value = mate_in;
      }
      if (mate_in < the_lowest_positive_value) {
        the_lowest_positive_value = mate_in;
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

short Engine::CalculateMovesToMate(
    std::vector<short> moves_to_mate, bool white_to_move) const {
  const bool is_my_move = playing_white_ == white_to_move;
  BorderValues border_values = GetBorderValuesForVector(moves_to_mate);
  short result = 0;
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
  return result;
}

void Engine::UpdateMoveEvalBasedOnChildren(EngineMove& move) const {
}

short Engine::EvaluateChildren(Board& board, EngineMove* moves, short number_of_children, bool remember_best_move) const {
  if (number_of_children == 0) {
    return IsMate(board) ? (board.WhiteToMove() ? -1 : 1) : 0;
  }
  std::vector<short> all_moves_to_mate;
  for (short i = 0; i < number_of_children; ++i) {
    EngineMove& move = moves[i];
    Board copy = board.Clone();
    MoveCalculator::ApplyMoveOnBoard(copy, move.move);
    short moves_to_mate = EvaluateChildren(copy, move.children, move.number_of_children, false);
    all_moves_to_mate.push_back(moves_to_mate);
  }
  short result = CalculateMovesToMate(all_moves_to_mate, board.WhiteToMove());
  if (remember_best_move) {
    for (size_t i = 0; i < all_moves_to_mate.size(); ++i) {
      if (all_moves_to_mate[i] == result) {
        best_move_ = moves[i].move.ToMove();
        break;
      }
    }
    assert(!best_move_.old_square.IsInvalid() && !best_move_.new_square.IsInvalid());
  }
  if (result > 0) {
    ++result;
  } else if (result < 0) {
    --result;
  }
  return result;
}

Move Engine::CalculateBestMove(Board& board) {
  assert(max_depth_ > 0u);
#ifdef ENGINE_DEBUG
  auto start_time = std::chrono::steady_clock::now();
#endif
  continue_calculations_ = true;
  nodes_calculated_ = 0u;
  utils::Timer timer;
  if (max_time_) {
    timer.start(max_time_, [this]() {
      continue_calculations_ = false;
    });
  }
  playing_white_ = board.WhiteToMove();
  short size;
  auto root = std::make_unique<EngineMove>();
  root->children = GenerateEngineMovesForBoard(board, size);
  root->number_of_children = size;
  if (root->number_of_children == 0) {
    GameResult result = GameResult::DRAW;
    const bool is_mate = board.IsKingInCheck(board.WhiteToMove());
    if (is_mate) {
      result = board.WhiteToMove() ? GameResult::BLACK_WON : GameResult::WHITE_WON;
    }
    throw NoMovesException(result);
  }
  for (size_t i = 1; i < max_depth_; ++i) {
#ifdef ENGINE_DEBUG
    std::cout << "Generating depth " << i << std::endl;
#endif
    GenerateNextDepth(board, root->children, root->number_of_children);
#ifdef ENGINE_DEBUG
    std::cout << "Depth " << i << " generated." << std::endl;
#endif
  }
#ifdef ENGINE_DEBUG
  auto mid_time = std::chrono::steady_clock::now();
  auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      mid_time - start_time).count();
  std::cout << "Going to evaluate. Time spent on generating moves: " << time_elapsed << "ms" << std::endl;
#endif
  EvaluateChildren(board, root->children, root->number_of_children, true);
#ifdef ENGINE_DEBUG
  auto end_time = std::chrono::steady_clock::now();
  time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      end_time - mid_time).count();
  std::cout << "Time spent on moves evaluation: " << time_elapsed << "ms" << std::endl;
#endif
  if (max_time_) {
    timer.stop();
  }
  return best_move_;
}
