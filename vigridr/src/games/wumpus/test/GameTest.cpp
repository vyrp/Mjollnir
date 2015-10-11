#include "../server/GameLogic.h"
#include "gtest/gtest.h"

namespace mjollnir { namespace vigridr {

class GameLogicTest : public ::testing::Test {
 protected:

  GameLogicTest() : game1(9090, 9091) { }

  std::vector<std::vector<WorldSquare>> CreateMap(){

    size_t size = game1.getWorldSize();    
    std::vector<std::vector<WorldSquare>> map;
    for(size_t i = 0; i < size; i++) {
      std::vector<WorldSquare> row;
      for(size_t j = 0; j < size; j++) {
        WorldSquare ws;
        row.push_back(ws);
      }
      map.push_back(row);
    }

    return map;
  }

  GameLogic game1;
};

TEST_F(GameLogicTest, TestingMapPropertiesFilling) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  ASSERT_TRUE(game1.getTotalWorldModel().map[0][0].wumpus);
  ASSERT_TRUE(game1.getTotalWorldModel().map[0][0].stench);
  ASSERT_TRUE(game1.getTotalWorldModel().map[0][1].stench);
  ASSERT_TRUE(game1.getTotalWorldModel().map[1][0].stench);
  
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][2].pit);
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][1].breeze);
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][3].breeze);
  ASSERT_TRUE(game1.getTotalWorldModel().map[2][2].breeze);

  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::RIGHT);

  ASSERT_TRUE(game1.getTotalWorldModel().map[1][2].gold);

}

TEST_F(GameLogicTest, TestingWalkingOnMap) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::RIGHT);

  Command command; command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][1].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::RIGHT);

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][1].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::UP);

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
  ASSERT_TRUE(game1.getTotalWorldModel().map[2][1].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::UP);

}


TEST_F(GameLogicTest, TestingBumpimpIntoWall) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::TURNRIGHT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::DOWN);

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::DOWN);
  ASSERT_TRUE(game1.getWorldModel().sensors.bump);
  ASSERT_FALSE(game1.getWorldModel().sensors.scream);
  ASSERT_FALSE(game1.getWorldModel().sensors.breeze);
  ASSERT_FALSE(game1.getWorldModel().sensors.glitter);
  ASSERT_FALSE(game1.getWorldModel().sensors.stench);

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::RIGHT);
  ASSERT_FALSE(game1.getWorldModel().sensors.bump);
  ASSERT_FALSE(game1.getWorldModel().sensors.scream);
  ASSERT_FALSE(game1.getWorldModel().sensors.breeze);
  ASSERT_FALSE(game1.getWorldModel().sensors.glitter);
  ASSERT_FALSE(game1.getWorldModel().sensors.stench);

}

TEST_F(GameLogicTest, TestingDyingOnPit) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::STAY;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getTotalWorldModel().map[3][2].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::RIGHT);
  ASSERT_FALSE(game1.getWorldModel().sensors.bump);
  ASSERT_FALSE(game1.getWorldModel().sensors.scream);
  ASSERT_FALSE(game1.getWorldModel().sensors.breeze);
  ASSERT_FALSE(game1.getWorldModel().sensors.glitter);
  ASSERT_FALSE(game1.getWorldModel().sensors.stench);

  ASSERT_EQ("s:-1003", game1.getWinner());

}


TEST_F(GameLogicTest, TestingDyingByWumpus) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getTotalWorldModel().map[0][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::UP);
  ASSERT_FALSE(game1.getWorldModel().sensors.bump);
  ASSERT_FALSE(game1.getWorldModel().sensors.scream);
  ASSERT_FALSE(game1.getWorldModel().sensors.breeze);
  ASSERT_FALSE(game1.getWorldModel().sensors.glitter);
  ASSERT_FALSE(game1.getWorldModel().sensors.stench);

  ASSERT_EQ("s:-1004", game1.getWinner());

}


TEST_F(GameLogicTest, TestingLeaveCaveWithoutGold) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::DOWN);
  ASSERT_FALSE(game1.getWorldModel().sensors.bump);
  ASSERT_FALSE(game1.getWorldModel().sensors.scream);
  ASSERT_FALSE(game1.getWorldModel().sensors.breeze);
  ASSERT_FALSE(game1.getWorldModel().sensors.glitter);
  ASSERT_FALSE(game1.getWorldModel().sensors.stench);

  ASSERT_EQ("s:-8", game1.getWinner());

}

TEST_F(GameLogicTest, TestingLeaveCaveWithGold) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNRIGHT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getWorldModel().sensors.glitter);

  command.action = Action::GRAB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
  
  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::DOWN);
  ASSERT_FALSE(game1.getWorldModel().sensors.bump);
  ASSERT_FALSE(game1.getWorldModel().sensors.scream);
  ASSERT_FALSE(game1.getWorldModel().sensors.breeze);
  ASSERT_FALSE(game1.getWorldModel().sensors.glitter);
  ASSERT_FALSE(game1.getWorldModel().sensors.stench);

  ASSERT_EQ("s:985", game1.getWinner());

}


TEST_F(GameLogicTest, TestingClimbingOnWrongSquare) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getTotalWorldModel().map[3][1].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::RIGHT);

  command.action = Action::STAY;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));
}

TEST_F(GameLogicTest, TestingShootingWumpusHearScream) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::SHOOT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getWorldModel().sensors.scream);

  command.action = Action::STAY;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_FALSE(game1.getWorldModel().sensors.scream);

  ASSERT_TRUE(game1.getTotalWorldModel().map[3][0].player);
  ASSERT_EQ(game1.getTotalWorldModel().playerDirection, Direction::UP);

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_EQ("s:-13", game1.getWinner());

}


TEST_F(GameLogicTest, TestingMissingShootingWumpus) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::SHOOT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_FALSE(game1.getWorldModel().sensors.scream);

  command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::SHOOT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_FALSE(game1.getWorldModel().sensors.scream);

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_EQ("s:-13", game1.getWinner());

}

TEST_F(GameLogicTest, TestingNotDyingOnWumpusAfterShootingIt) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[1][2].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::SHOOT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getWorldModel().sensors.scream);

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getWorldModel().sensors.stench);

  command.action = Action::TURNRIGHT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNRIGHT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_EQ("s:-20", game1.getWinner());

}


TEST_F(GameLogicTest, TestingGettingGoldOnWumpusSquare) {

  std::vector<std::vector<WorldSquare>> map = CreateMap();
  map[0][0].wumpus = true;
  game1.setWumpusPosition(0, 0);
  map[3][2].pit = true;
  map[0][0].gold = true;

  game1.initializeWorld(map);

  Command command; command.action = Action::TURNLEFT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::SHOOT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getWorldModel().sensors.scream);

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  ASSERT_TRUE(game1.getWorldModel().sensors.stench);
  ASSERT_TRUE(game1.getWorldModel().sensors.glitter);

  command.action = Action::GRAB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNRIGHT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::TURNRIGHT;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::FORWARD;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_TRUE(game1.update(command, 9091));

  command.action = Action::CLIMB;
  ASSERT_TRUE(game1.update(command, 9090));
  ASSERT_FALSE(game1.update(command, 9091));

  ASSERT_EQ("s:979", game1.getWinner());

}

}}  // namespaces

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}