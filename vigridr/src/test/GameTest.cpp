#include "../server/GameLogic.h"
#include "gtest/gtest.h"

namespace mjollnir { namespace vigridr {

class GameLogicTest : public ::testing::Test {
 protected:

  GameLogicTest() : game1(9090, 9091) { }

  // void CleanTable() {
  //   Coordinate coord;
  //   for(size_t i = 0; i < 3; ++i) {
  //     for(size_t j = 0; j < 3; ++j) {
  //       coord.x = i; coord.y = j;
  //       game1.setTableCoordinate(coord, Marker::UNMARKED);
  //     }
  //   }
  //   game1.setHasFinished(false);
  //   game1.setWinner(-1);
  // }

  GameLogic game1;
};

// TEST_F(GameLogicTest, TestingPlayersMarkersTypes) {
//   Command command1; command1.coordinate.x = 1; command1.coordinate.y = 1;
//   Command command2; command2.coordinate.x = 0; command2.coordinate.y = 2;
//   ASSERT_TRUE(game1.update(command1, 9090));
//   ASSERT_TRUE(game1.update(command2, 9091));

//   std::vector<std::vector<Marker> > expTable {
//       {Marker::UNMARKED, Marker::UNMARKED, Marker::O},
//       {Marker::UNMARKED, Marker::X,        Marker::UNMARKED},
//       {Marker::UNMARKED, Marker::UNMARKED, Marker::UNMARKED}
//   };
//   ASSERT_EQ(expTable, game1.getWorldModel().table);
// }

// TEST_F(GameLogicTest, TestingTwoCommandsInTheSameTableEntry) {
//   Command command1; command1.coordinate.x = 1; command1.coordinate.y = 1;
//   Command command2; command2.coordinate.x = 0; command2.coordinate.y = 2;
//   game1.update(command1, 9090);
//   game1.update(command2, 9091);
//   ASSERT_FALSE(game1.update(command1, 9090));
//   ASSERT_FALSE(game1.update(command2, 9091));
//   std::vector<std::vector<Marker> > expTable {
//       {Marker::UNMARKED, Marker::UNMARKED, Marker::O},
//       {Marker::UNMARKED, Marker::X,        Marker::UNMARKED},
//       {Marker::UNMARKED, Marker::UNMARKED, Marker::UNMARKED}
//   };
//   ASSERT_EQ(expTable, game1.getWorldModel().table);

//   ASSERT_FALSE(game1.update(command1, 9091));
//   ASSERT_FALSE(game1.update(command2, 9090));
//   ASSERT_EQ(expTable, game1.getWorldModel().table);
// }

// TEST_F(GameLogicTest, TestDraw) {
//   std::vector<std::vector<int> > indexes {
//     {2,2,9090},
//     {0,0,9091},
//     {0,1,9090},
//     {1,1,9091},
//     {0,2,9090},
//     {1,2,9091},
//     {1,0,9090},
//     {2,1,9091},
//     {2,0,9090}
//   };
//   Command command;
//   for(const auto& line : indexes) {
//     command.coordinate.x = line[0];
//     command.coordinate.y = line[1];
//     game1.update(command, line[2]);
//   }
//   EXPECT_TRUE(game1.isFinished());
//   ASSERT_TRUE(game1.getWinner() == -1);
// }

// TEST_F(GameLogicTest, TestWinningTables) {
//   std::vector<std::vector<int> > indexes {
//     {0,1,9091},
//     {0,2,9091},
//     {0,0,9090},
//     {1,1,9090},
//     {2,2,9090}
//   };
//   Command command;
//   for(const auto& line : indexes) {
//     command.coordinate.x = line[0];
//     command.coordinate.y = line[1];
//     game1.update(command, line[2]);
//   }
//   EXPECT_TRUE(game1.isFinished());
//   ASSERT_TRUE(game1.getWinner() == 9090);

//   CleanTable();
//   indexes = std::vector<std::vector<int> > {
//     {0,0,9090},
//     {0,1,9090},
//     {0,2,9091},
//     {1,2,9091},
//     {2,2,9091}
//   };
//   for(const auto& line : indexes) {
//     command.coordinate.x = line[0];
//     command.coordinate.y = line[1];
//     game1.update(command, line[2]);
//   }
//   EXPECT_TRUE(game1.isFinished());
//   ASSERT_TRUE(game1.getWinner() == 9091);
// }

// TEST_F(GameLogicTest, TwoWinners) {
//    std::vector<std::vector<int> > indexes {
//     {0,0,9090},
//     {1,0,9091},
//     {0,1,9090},
//     {1,1,9091},
//     {0,2,9090},
//     {1,2,9091}
//   };
//   Command command;
//   for(const auto& line : indexes) {
//     command.coordinate.x = line[0];
//     command.coordinate.y = line[1];
//     game1.update(command, line[2]);
//   }
//   EXPECT_TRUE(game1.isFinished());
//   ASSERT_TRUE(game1.getWinner() == 9090);
// }

}}  // namespaces

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}