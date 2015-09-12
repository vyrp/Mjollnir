#include "GameLogger.h"

#include <fstream>
#include <vector>
#include <array>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

namespace mjollnir { namespace vigridr {

std::vector<WorldModel> wmList;
GameDescription gd1,gd2;
std::string player1Id,player2Id;
char symbols[] = "XO";


ptree createGameDescriptionPt() {
  ptree gdPt;
  gdPt.put(player1Id, std::to_string(gd1.myIndex));
  gdPt.put(player2Id, std::to_string(gd2.myIndex));
  gdPt.put("fieldWidth", std::to_string(gd1.field.width));
  gdPt.put("fieldHeight", std::to_string(gd1.field.height));
  return gdPt;
}

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

void GameLogger::printWorldModel(const WorldModel& wm) {
  const int32_t w = gd1.field.width;
  const int32_t h = gd1.field.height;
  
  std::vector<std::vector<char>> field(h, std::vector<char>(w+2, '.'));
  for (int i=0; i<h; i++) {
    field[i][w] = '\n';
    field[i][w+1] = '\0';
  }
  
  for (size_t i=0; i<wm.players.size(); i++) {
    const auto& player = wm.players[i];
    for (const auto& coor : player.body) {
      field[coor.y][coor.x] = symbols[i];
    }
  }
  
  std::ostringstream oss;
  for (int i=0; i<h; i++) {
    oss << field[i].data();
  }
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
