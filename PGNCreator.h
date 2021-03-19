#ifndef PGN_CREATOR_H
#define PGN_CREATOR_H

#include <string>

#include "Board.h"
#include "MoveCalculator.h"


class PGNCreator {
 public:
  enum class Result {
    NONE,
    WHITE_WON,
    BLACK_WON,
    DRAW
  };

  std::string GetPGN() const;
  void GameFinished(Result result);
  std::string AddMove(const Board& board, const Move& move);

 private:
  Result result_{Result::NONE};
};

#endif  // PGN_CREATOR_H
