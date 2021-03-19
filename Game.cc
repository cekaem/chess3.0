#include "Board.h"
#include "Engine.h"

int main() {
  try {
    Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Engine engine;
    while (1) {
      Move move = engine.CalculateBestMove(board);
      std::cout << move << std::endl;
      board = move.board;
    }
  } catch (NoMovesException& e) {
  }
  return 0;
}
