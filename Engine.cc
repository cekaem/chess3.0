#include "Engine.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>


namespace {

static double PawnValue = 1.0;
static double KnightValue = 3.0;
static double BishopValue = 3.0;
static double RookValue = 5.0;
static double QueenValue = 8.0;

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

bool IsMate(const Board& board) {
  MoveCalculator calculator;
  return calculator.CalculateAllMoves(board).empty() &&
         board.IsKingInCheck(board.WhiteToMove());

}

}  // unnamed namespace


Engine::EngineMove::EngineMove(const Board& b) : board(std::move(b)) {}

void Engine::EngineMove::Evaluate() {
  eval = EvaluateMove(board);
  mate_in = IsMate(board) ? (board.WhiteToMove() ? -1 : 1) : 0;
}

Engine::Engine(unsigned depth, unsigned time)
  : max_depth_(depth),
    max_time_(time)  {
  srand(static_cast<unsigned int>(clock()));
}

Engine::EngineMoves Engine::GenerateEngineMovesForBoard(const Board& board) {
  EngineMoves result;
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(board);
  for (const auto& move: moves) {
    result.push_back(EngineMove(move.board));
  }
  nodes_calculated_ += result.size();
  return result;
}

Move Engine::FindMoveForBoard(const Board& initial_board, const Board& dest_board) const {
  MoveCalculator calculator;
  auto moves = calculator.CalculateAllMoves(initial_board);
  auto iter = std::find_if(moves.begin(), moves.end(), [dest_board](const Move& move) {
    return move.board == dest_board;
  });
  assert(iter != moves.end());
  return *iter;
}

void Engine::GenerateNextDepth(EngineMoves& moves) {
  if (!continue_calculations_) {
    return;
  }
  for (auto& move: moves) {
    if (move.children.empty()) {
      move.children = GenerateEngineMovesForBoard(move.board);
    } else {
      GenerateNextDepth(move.children);
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

bool Engine::UpdateMoveMovesToMateBasedOnChildren(EngineMove& move) const {
  int mate_for_white = 1000;
  int mate_for_black = -1000;
  for (const auto& child: move.children) {
    if (child.mate_in > 0 && child.mate_in < mate_for_white) {
      mate_for_white = child.mate_in + 1;
    } else if (child.mate_in < 0 && child.mate_in > mate_for_black) {
      mate_for_black = child.mate_in - 1;
    }
  }
}

void Engine::UpdateMoveEvalBasedOnChildren(EngineMove& move) const {
}

void Engine::EvaluateChildrenAndUpdateParent(EngineMoves& parent) const {
  for (auto& move: parent) {
    if (move.children.empty()) {
      move.Evaluate();
    } else {
      EvaluateChildrenAndUpdateParent(move.children);
      if (!UpdateMoveMovesToMateBasedOnChildren(move)) {
        UpdateMoveEvalBasedOnChildren(move);
      }
    }
  }
}

Move Engine::CalculateBestMove(const Board& board) {
  assert(max_depth_ > 0u);
  assert(max_time_ > 0u);
  continue_calculations_ = true;
  nodes_calculated_ = 0u;
  timer_.start(max_time_, [this]() {
      continue_calculations_ = false;
  });
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
    GenerateNextDepth(root_);
  }
  EvaluateChildrenAndUpdateParent(root_);
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
  timer_.stop();
  std::cout << "nodes: " << nodes_calculated_ << std::endl;
  return FindMoveForBoard(board, best_moves[index].board);
}
