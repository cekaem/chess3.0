#ifndef TYPES_H
#define TYPES_H

#include <iostream>


enum class GameResult {
  NONE,
  WHITE_WON,
  BLACK_WON,
  DRAW
};

inline std::ostream& operator<<(std::ostream& ostr, GameResult result) {
  switch (result) {
    case GameResult::NONE:
      ostr << "NONE";
      break;
    case GameResult::WHITE_WON:
      ostr << "WHITE_WON";
      break;
    case GameResult::BLACK_WON:
      ostr << "BLACK WON";
      break;
    case GameResult::DRAW:
      ostr << "DRAW";
      break;
  }
  return ostr;
}

#endif  // TYPES_H
