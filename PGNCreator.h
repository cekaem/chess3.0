#ifndef PGN_CREATOR_H
#define PGN_CREATOR_H

#include <string>
#include <vector>

#include "Board.h"
#include "MoveCalculator.h"
#include "Types.h"

class PGNCreator {
 public:
  std::string GetPGN() const;
  void GameFinished(GameResult result);
  std::string AddMove(const Board& board, const SerializedMove& move);

 private:
  GameResult result_{GameResult::NONE};
  std::vector<std::string> moves_;
};

#endif  // PGN_CREATOR_H
