#include <iostream>

#include "Board.h"
#include "Engine.h"
#include "PGNCreator.h"
#include "Types.h"


int main() {
  PGNCreator pgn_creator;
  try {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Engine engine;
    for (int i = 0; i < 100; ++i) {
      Move move = engine.CalculateBestMove(board);
      pgn_creator.AddMove(board, move);
      std::cout << move << std::endl;
      board = move.board;
    }
    pgn_creator.GameFinished(GameResult::DRAW);
  } catch (NoMovesException& e) {
    pgn_creator.GameFinished(e.result);
  }
  std::cout << pgn_creator.GetPGN() << std::endl;
  return 0;
}
