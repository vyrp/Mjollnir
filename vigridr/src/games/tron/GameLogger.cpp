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

ptree createPt(const WorldModel& wm) {
  ptree wmPt;
  ptree playersPt;
  for (auto player : wm.players) {
    ptree playerPt;
    ptree bodyPt;
    for (auto piece : player.body) { 
      ptree piecePt;
      piecePt.push_back(std::make_pair("x", ptree(std::to_string(piece.x))));
      piecePt.push_back(std::make_pair("y", ptree(std::to_string(piece.y))));
      bodyPt.push_back(std::make_pair("", piecePt));
    }
    playerPt.push_back(std::make_pair("body", bodyPt));
    playersPt.push_back(std::make_pair("", playerPt));
  }
  wmPt.push_back(std::make_pair("players",playersPt));
  return wmPt;
}

void GameLogger::logWorldModel(const WorldModel& wm) {
  wmList.push_back(wm);
}

void GameLogger::flushLog() {
  ptree wmListPt;
  for (auto wm : wmList) {
    wmListPt.push_back(std::make_pair("", createPt(wm)));
  }
  ptree gamePt;
  gamePt.push_back(std::make_pair("wmList", wmListPt));
  std::ofstream file;
  file.open("logs"); 
  write_json (file, gamePt, false);
  file.close();
}

}}