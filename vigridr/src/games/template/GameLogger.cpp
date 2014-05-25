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
  // TODO: implement wm -> ptree
  return ptree(std::to_string(wm.sampleData));
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
  write_json (file, gamePt, true);
  file.close();
}

}}