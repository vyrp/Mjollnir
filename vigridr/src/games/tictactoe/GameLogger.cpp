#include "GameLogger.h"

#include <fstream>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

namespace mjollnir { namespace vigridr {

std::vector<WorldModel> wmList;
GameDescription gd1,gd2;
std::string player1Id,player2Id;

ptree createPt(const WorldModel& wm) {
  ptree wmPt;
  ptree tablePt;
  for (auto line : wm.table) {
    ptree linePt;
    for (auto elem : line) {
      std::string elemStr =
        (elem == Marker::O) ? "O" : (elem == Marker::X) ? "X" : "-";
      linePt.push_back(std::make_pair("", ptree(elemStr)));
    }
    tablePt.push_back(std::make_pair("", linePt));
  }
  wmPt.push_back(std::make_pair("table",tablePt));
  return wmPt;
}

ptree createGameDescriptionPt() {
  ptree gdPt;
  gdPt.put(player1Id, gd1.myType == Marker::O ? "O":"X");
  gdPt.put(player2Id, gd2.myType == Marker::O ? "O":"X");
  return gdPt;
}

void GameLogger::logWorldModel(const WorldModel& wm) {
  wmList.push_back(wm);
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