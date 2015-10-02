#include "GameLogger.h"

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

namespace mjollnir { namespace vigridr {

std::vector<WorldModel> wmList;
std::vector<TotalWorldModel> twmList;
GameDescription gd1, gd2;
std::string player1Id, player2Id;

std::string worldSquareStr(WorldSquare elem){
  std::string elemStr;

  elemStr = "";
  if(elem.player) elemStr = elemStr + "A-";
  else elemStr = elemStr + " -";
  if(elem.breeze) elemStr = elemStr + "B-";
  else elemStr = elemStr + " -";
  if(elem.gold) elemStr = elemStr + "G-";
  else elemStr = elemStr + " -";
  if(elem.pit) elemStr = elemStr + "P-";
  else elemStr = elemStr + " -";
  if(elem.stench) elemStr = elemStr + "S-";
  else elemStr = elemStr + " -";
  if(elem.wumpus) elemStr = elemStr + "W";
  else elemStr = elemStr + " ";

  return elemStr;
}

std::string playerDirectionStr(Direction playerDirection){

  std::string directionStr;

  switch(playerDirection){
    case UP:
      directionStr = "UP";
      break;
    case RIGHT:
      directionStr = "RIGHT";
      break;
    case DOWN:
      directionStr = "DOWN";
      break;
    case LEFT:
      directionStr = "LEFT";
      break;
  }

  return directionStr;
}


void GameLogger::printWorldModel(const WorldModel& wm, const TotalWorldModel& twm) {
  std::ostringstream oss;
  std::string l = " ---------------------------------------------------------";
  oss << l << std::endl;
  for (auto line : twm.map) {
    oss << " | ";
    for (auto elem : line) {
      oss << worldSquareStr(elem) << " | ";
    }
    oss << std::endl << l <<std::endl;
  }
  oss << "Player direction: " << playerDirectionStr(twm.playerDirection);
  oss << std::endl;
  oss << "A: Adventurer  B: Breeze  G: Gold" << std::endl;
  oss << "P: Pit  S: Stench  W: Wumpus" << std::endl;
  std::cerr << oss.str();
}


ptree createPt(const TotalWorldModel& twm) {
  ptree twmPt;

  ptree tablePt;
  for (auto line : twm.map) {
    ptree linePt;
    for (auto elem : line) {
      linePt.push_back(std::make_pair("", ptree(worldSquareStr(elem))));
    }
    tablePt.push_back(std::make_pair("", linePt));
  }
  
  twmPt.push_back(std::make_pair("table",tablePt));
  twmPt.push_back(std::make_pair("direction", ptree(playerDirectionStr(twm.playerDirection))));

  return twmPt;
}

ptree createGameDescriptionPt() {
  ptree gdPt;
  gdPt.put(player1Id, gd1.playerType == PlayerType::PLAYER ? "PLAYER":"COMPUTER");
  gdPt.put(player2Id, gd2.playerType == PlayerType::PLAYER ? "PLAYER":"COMPUTER");
  return gdPt;
}
void GameLogger::logWorldModel(const WorldModel& wm, const TotalWorldModel& twm) {
  wmList.push_back(wm);
  twmList.push_back(twm);
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
  ptree twmListPt;
  int n = 0;

  for (auto twm : twmList) {
    if(n % 2 == 0){
      twmListPt.push_back(std::make_pair("", createPt(twm)));
    }
    n++;
  }
  ptree gamePt;
  gamePt.push_back(std::make_pair("wmList", twmListPt));
  gamePt.push_back(std::make_pair("gameDescription",
                                  createGameDescriptionPt()));
  std::ofstream file;
  file.open("logs");
  write_json (file, gamePt, false);
  file.close();
}

}}