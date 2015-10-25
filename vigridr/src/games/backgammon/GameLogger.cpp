#include "GameLogger.h"

#include <fstream>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using std::endl;
using std::string;
using std::vector;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

namespace mjollnir { namespace vigridr {

const size_t LINES = 19;
const size_t COLUMNS = 53;

vector<WorldModel> wmList;
GameDescription gd1,gd2;
string player1Id, player2Id;

ptree createGameDescriptionPt() {
  ptree gdPt;
  gdPt.put(player1Id, gd1.myColor == PlayerColor::WHITE ? "WHITE":"RED");
  gdPt.put(player2Id, gd2.myColor == PlayerColor::WHITE ? "WHITE":"RED");
  return gdPt;
}

ptree pointToPtree(Point p) {
  ptree pointPt;
  pointPt.put("reds", std::to_string(p[RED]));
  pointPt.put("whites", std::to_string(p[WHITE]));
  return pointPt;
}

ptree createPt(const WorldModel& wm) {
  ptree wmPt;

  wmPt.push_back(std::make_pair("bar", pointToPtree(wm.bar)));
  wmPt.push_back(std::make_pair("borne_off", pointToPtree(wm.borne_off)));

  ptree dicePt;
  dicePt.push_back(std::make_pair("", ptree(std::to_string(wm.dice[0]))));
  dicePt.push_back(std::make_pair("", ptree(std::to_string(wm.dice[1]))));
  wmPt.push_back(std::make_pair("dice", dicePt));

  ptree boardPt;
  for (const auto& point : wm.board) {
    boardPt.push_back(std::make_pair("", pointToPtree(point)));
  }
  wmPt.push_back(std::make_pair("board", boardPt));

  return wmPt;
}

void calculate_coordinates(size_t i, size_t& row, size_t& column, int& direction) {
  if (i < 6) {
    row = LINES - 4;
    column = COLUMNS - 4 - i * 4;
    direction = -1;
  } else if (i < 12) {
    row = LINES - 4;
    column = COLUMNS - 6 - i * 4;
    direction = -1;
  } else if (i < 18) {
    row = 3;
    column = 3 + (i - 12) * 4;
    direction = 1;
  } else {
    row = 3;
    column = 5 + (i - 12) * 4;
    direction = 1;
  }
}

void GameLogger::logWorldModel(const WorldModel& wm, const TotalWorldModel& twm) {
  wmList.push_back(wm);
}

void GameLogger::printWorldModel(const WorldModel& wm, const TotalWorldModel& twm) {
  std::ostringstream oss;

  vector<string> board {
    "++---+---+---+---+---+---+++---+---+---+---+---+---++",
    "|| 12| 13| 14| 15| 16| 17||| 18| 19| 20| 21| 22| 23||",
    "++---+---+---+---+---+---+++---+---+---+---+---+---++",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "||   |   |   |   |   |   |||   |   |   |   |   |   ||",
    "++---+---+---+---+---+---+++---+---+---+---+---+---++",
    "|| 11| 10| 9 | 8 | 7 | 6 ||| 5 | 4 | 3 | 2 | 1 | 0 ||",
    "++---+---+---+---+---+---+++---+---+---+---+---+---++"
  };

  for (size_t i = 0; i < wm.board.size(); ++i) {
    size_t row, column;
    int direction;
    calculate_coordinates(i, row, column, direction);

    size_t numberOfItems;
    char playerColor;
    if (wm.board[i][RED] != 0) {
      numberOfItems = wm.board[i][RED];
      playerColor = 'R';
    } else {
      numberOfItems = wm.board[i][WHITE];
      playerColor = 'W';
    }

    for (size_t j = 0; j < numberOfItems; ++j) {
      board[row + j * direction][column] = playerColor;
      if (j == 5) {
        board[row + j * direction][column - 1] = '+';
        numberOfItems -= 5;
        if (numberOfItems == 10) {
          board[row + j * direction][column] = '1';
          board[row + j * direction][column+1] = '0';
        } else {
          board[row + j * direction][column] = '0' + numberOfItems;
        }
        break;
      }
    }
  }

  for (size_t i = 0; i < board.size(); ++i) {
    oss << board[i];
    if (i == 4) {
      oss << " => REDS (" << wm.borne_off[RED] << ")";
    } else if (i == LINES - 5) {
      oss << " => WHITES (" << wm.borne_off[WHITE] << ")";
    }
    oss << endl;
  }

  oss << "Bar  - R: " << wm.bar[RED] <<", W: " << wm.bar[WHITE] << endl;
  oss << "Dice - " << wm.dice[0] << ", " << wm.dice[1] << endl << endl;

  std::cerr << oss.str();
}

void GameLogger::logGameDescription(const GameDescription& description1,
                                    const std::string& player1,
                                    const GameDescription& description2,
                                    const std::string& player2) {
  gd1 = description1;
  gd2 = description2;
  player1Id = player1;
  player2Id = player2;
}

void GameLogger::flushLog() {
  ptree wmListPt;
  for (auto wm : wmList) {
    wmListPt.push_back(std::make_pair("", createPt(wm)));
  }
  ptree gamePt;
  gamePt.push_back(std::make_pair("wmList", wmListPt));
  gamePt.push_back(std::make_pair("gameDescription",
                                  createGameDescriptionPt()));
  std::ofstream file;
  file.open("logs");
  write_json (file, gamePt, false);
  file.close();
}

}}
