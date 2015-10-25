#include "../server/GameLogic.h"
#include "gtest/gtest.h"

#include <climits>   // for CHAR_BIT
#include <iostream>
#include <utility>
#include <vector>
#include <unordered_set>

using std::vector;
using std::pair;
using std::unordered_set;

/* Helper functions */

namespace mjollnir { namespace vigridr {

Command make_command(vector<pair<int32_t, int32_t>> moves) {
  Command command;
  for (const auto& move : moves) {
    command.moves.push_back(make_move(move.first, move.second));
  }
  return command;
}

size_t rotl(size_t value, size_t amount) {
  return (value << amount) | (value >> (sizeof(value) * CHAR_BIT - amount));
}

}}

namespace std {
  template<> struct hash< ::mjollnir::vigridr::Move >
  {
    std::size_t operator()(const ::mjollnir::vigridr::Move& m) const
    {
      return std::hash<int32_t>()(m.src) ^ (std::hash<int32_t>()(m.dst) << 1);
    }
  };
  template<> struct hash< ::mjollnir::vigridr::Command >
  {
    std::size_t operator()(const ::mjollnir::vigridr::Command& c) const
    {
      size_t ret = 0;
      for (size_t i = 0; i < c.moves.size(); ++i) {
        ret ^= ::mjollnir::vigridr::rotl(std::hash< ::mjollnir::vigridr::Move >()(c.moves[i]), i);
      }
      return ret;
    }
  };
}

namespace mjollnir { namespace vigridr {

/* Test classes */

class GameLogicTest : public ::testing::Test, protected GameLogic {
 protected:

  GameLogicTest() :
    GameLogic(9090, 9091),
    EMPTY_BOARD(24, {0, 0}) { }

  const vector<Point> EMPTY_BOARD;
};

class GameLogic_ExpectedTrue_Test : public GameLogicTest {
};

class GameLogic_ExpectedFalse_Test : public GameLogicTest {
};

class GameLogic_Functions_Test : public GameLogicTest {
};

class GameLogic_IntegrationAndStress_Test : public GameLogicTest {
};

/* Test functions */

TEST_F(GameLogic_ExpectedTrue_Test, InitializationTest) {
  std::vector<Point> expectedBoard {
    {2, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 5},

    {0, 0},
    {0, 3},
    {0, 0},
    {0, 0},
    {0, 0},
    {5, 0},

    {0, 5},
    {0, 0},
    {0, 0},
    {0, 0},
    {3, 0},
    {0, 0},

    {5, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 2}
  };
  Point expectedBar({0, 0});
  Point expectedBorneOff({0, 0});
  const size_t expectedDiceSize = 2;

  const auto& wm = getWorldModel();
  ASSERT_EQ(expectedBar, wm.bar);
  ASSERT_EQ(expectedBoard, wm.board);
  ASSERT_EQ(expectedBorneOff, wm.borne_off);
  ASSERT_EQ(expectedDiceSize, wm.dice.size());
  for (size_t i = 0; i < expectedDiceSize; ++i) {
    ASSERT_GE(wm.dice[i], 1);
    ASSERT_LE(wm.dice[i], 6);
  }
}

/*
 * Correct commands (allowed or mandatory moves)
 */

TEST_F(GameLogic_ExpectedTrue_Test, DifferentCheckersBasicMovement) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[12][RED] = 2;
  board[11][WHITE] = 2;
  board[8][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  // White move
  setDice_forTest({1, 2});
  ASSERT_TRUE(update(make_command({{11, 9}, {8, 7}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[9]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);

  ASSERT_FALSE(isFinished());

  // Red move
  setDice_forTest({3, 2});
  ASSERT_TRUE(update(make_command({{12, 14}, {12, 15}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[15]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[14]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[9]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, SameCheckerBasicMovement) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[12][RED] = 2;
  board[11][WHITE] = 2;
  board[8][WHITE] = 1;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  // White move
  setDice_forTest({1, 2});
  ASSERT_TRUE(update(make_command({{11, 9}, {9, 8}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[8]);

  ASSERT_FALSE(isFinished());

  // Red move
  setDice_forTest({3, 2});
  ASSERT_TRUE(update(make_command({{12, 15}, {15, 17}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[17]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[15]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[8]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, DoubleDiceCorrect) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[12][RED] = 2;
  board[11][WHITE] = 2;
  board[8][WHITE] = 2;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 11}), game_wm.borne_off);

  // White move
  setDice_forTest({2, 2});
  ASSERT_TRUE(update(make_command({{11, 9}, {11, 9}, {8, 6}, {8, 6}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[9]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[6]);

  ASSERT_FALSE(isFinished());

  // Red move
  setDice_forTest({2, 2});
  ASSERT_TRUE(update(make_command({{12, 14}, {14, 16}, {16, 18}, {18, 20}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[9]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[6]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 11}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, DoubleDiceCorrect_OneMove) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[11][WHITE] = 2;
  board[9][RED] = 2;
  board[8][WHITE] = 2;
  board[7][WHITE] = 1;
  board[6][RED] = 2;
  board[3][RED] = 2;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 2}), game_wm.board[11]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[6]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[3]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{9, 10}), game_wm.borne_off);

  // White move
  setDice_forTest({2, 2});
  ASSERT_TRUE(update(make_command({{7, 5}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 2}), game_wm.board[11]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[7]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[6]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[5]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[3]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{9, 10}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, JumpingOver) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[8][RED] = 2;
  board[6][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[6]);

  // White move
  setDice_forTest({5, 3});
  ASSERT_TRUE(update(make_command({{10, 5}, {5, 2}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 2}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[6]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);

  ASSERT_FALSE(isFinished());

  // Red move
  setDice_forTest({2, 3});
  ASSERT_TRUE(update(make_command({{8, 11}, {11, 13}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[13]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[11]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[10]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[6]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 11}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, BearingOffCorrectly) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[19][RED] = 2;
  board[4][WHITE] = 2;
  board[2][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  // White move
  setDice_forTest({5, 3});
  ASSERT_TRUE(update(make_command({{4, BEAR_OFF}, {2, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{13, 14}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // Red move
  setDice_forTest({4, 5});
  ASSERT_TRUE(update(make_command({{19, BEAR_OFF}, {19, 23}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[23]);
  ASSERT_EQ((Point{14, 14}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{14, 14}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, BearingOffCorrectly_MustHigher) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[19][RED] = 2;
  board[4][WHITE] = 2;
  board[2][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  // White move
  setDice_forTest({4, 3});
  ASSERT_TRUE(update(make_command({{2, BEAR_OFF}, {4, 0}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[0]);
  ASSERT_EQ((Point{13, 13}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());
}

TEST_F(GameLogic_ExpectedTrue_Test, ChoosingNotToBearOff) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[19][RED] = 2;
  board[4][WHITE] = 2;
  board[2][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  // White move
  setDice_forTest({4, 3});
  ASSERT_TRUE(update(make_command({{4, 1}, {4, 0}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[0]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());
}

TEST_F(GameLogic_ExpectedTrue_Test, BearingOffHighest) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[9][RED] = 2;
  board[5][RED] = 2;
  board[2][WHITE] = 2;
  board[1][WHITE] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[1]);
  ASSERT_EQ((Point{11, 11}), game_wm.borne_off);

  // White move
  setDice_forTest({5, 4});
  ASSERT_TRUE(update(make_command({{2, BEAR_OFF}, {2, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[1]);
  ASSERT_EQ((Point{11, 13}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
}

TEST_F(GameLogic_ExpectedTrue_Test, BearingOff_OnlyOnePiece) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[20][RED] = 1;
  board[4][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{14, 14}), game_wm.borne_off);

  // White move
  setDice_forTest({5, 2});
  ASSERT_TRUE(update(make_command({{4, 2}, {2, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{14, 15}), game_wm.borne_off);

  ASSERT_TRUE(isFinished());
}

TEST_F(GameLogic_ExpectedTrue_Test, CannotBearOff_MustUseTwoDice) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[20][RED] = 1;
  board[5][WHITE] = 1;
  board[4][RED] = 2;
  board[1][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[5]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{12, 13}), game_wm.borne_off);

  // White move
  setDice_forTest({1, 2});
  ASSERT_TRUE(update(make_command({{5, 3}, {1, 0}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[3]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[0]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{12, 13}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());
}

TEST_F(GameLogic_ExpectedTrue_Test, HittingAndComingBackCorrectly) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[14][WHITE] = 1;
  board[12][RED] = 1;
  board[10][WHITE] = 2;
  board[7][RED] = 1;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[14]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[10]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[7]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  // White move
  setDice_forTest({1, 2});
  ASSERT_TRUE(update(make_command({{10, 8}, {8, 7}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[14]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);
  ASSERT_EQ((Point{1, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // Red move
  setDice_forTest({3, 2});
  ASSERT_TRUE(update(make_command({{FROM_BAR, 2}, {12, 14}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[14]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());
}

TEST_F(GameLogic_ExpectedTrue_Test, DoubleHitAndDoubleComeBack) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 1;
  board[8][WHITE] = 1;
  board[7][WHITE] = 1;
  board[5][RED] = 2;
  setBoard_forTest(board);
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 0}), game_wm.bar);

  // Initial state
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);

  // Red move
  setDice_forTest({2, 3});
  ASSERT_TRUE(update(make_command({{5, 7}, {5, 8}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[7]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.bar);

  ASSERT_FALSE(isFinished());

  // White move
  setDice_forTest({1, 6});
  ASSERT_TRUE(update(make_command({{FROM_BAR, 23}, {FROM_BAR, 18}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[23]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[7]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, EndOfGame) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][RED] = 1;
  board[2][WHITE] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{14, 13}), game_wm.borne_off);

  // White move
  setDice_forTest({3, 4});
  ASSERT_TRUE(update(make_command({{2, BEAR_OFF}, {2, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{14, 15}), game_wm.borne_off);

  ASSERT_TRUE(isFinished());
  ASSERT_EQ("9091", getWinner());
}

TEST_F(GameLogic_ExpectedTrue_Test, EndOfGameDoubleDice) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][RED] = 3;
  board[4][WHITE] = 1;
  board[3][WHITE] = 1;
  board[2][WHITE] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{3, 0}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[3]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{12, 11}), game_wm.borne_off);

  // White move
  setDice_forTest({5, 5});
  ASSERT_TRUE(update(make_command({{4, BEAR_OFF}, {3, BEAR_OFF}, {2, BEAR_OFF}, {2, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{3, 0}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[3]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{12, 15}), game_wm.borne_off);

  ASSERT_TRUE(isFinished());
  ASSERT_EQ(getWinner(), "9091");
}

TEST_F(GameLogic_ExpectedTrue_Test, OnlyOneOption) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 2;
  board[8][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 2}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_TRUE(update(make_command({{10, 7}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 13}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, OnlyOneOption_MustHigher) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 1;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_TRUE(update(make_command({{10, 7}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 0}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 14}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, NoMove) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({1, 5});
  ASSERT_TRUE(update(make_command({}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, NoMove_FromBar) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[23][RED] = 2;
  board[18][RED] = 2;
  board[10][WHITE] = 1;
  board[8][WHITE] = 1;
  board[7][WHITE] = 1;
  board[5][RED] = 2;
  setBoard_forTest(board);
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 0}), game_wm.bar);

  // Initial state
  ASSERT_EQ((Point{2, 0}), game_wm.board[23]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[8]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[7]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);

  // Red move
  setDice_forTest({2, 3});
  ASSERT_TRUE(update(make_command({{5, 7}, {5, 8}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[23]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[7]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.bar);

  ASSERT_FALSE(isFinished());

  // White move
  setDice_forTest({1, 6});
  ASSERT_TRUE(update(make_command({}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[23]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[8]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[7]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.bar);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{9, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedTrue_Test, MandatoryMove) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[4][WHITE] = 1;
  board[3][WHITE] = 1;
  board[1][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[3]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[1]);
  ASSERT_EQ((Point{13, 13}), game_wm.borne_off);

  // White move
  setDice_forTest({5, 2});
  ASSERT_TRUE(update(make_command({{4, 2}, {3, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 0}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[3]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[1]);
  ASSERT_EQ((Point{13, 14}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
}

/*
 * Incorrect commands (prohibited moves)
 */

TEST_F(GameLogic_ExpectedFalse_Test, InsuficientMoves) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{10, 8}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, TooManyMoves) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{10, 8}, {10, 7}, {10, 6}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, MovesNotInDice) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{10, 8}, {10, 6}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, BlockedMove) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({1, 3});
  ASSERT_FALSE(update(make_command({{10, 9}, {10, 7}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, MoveFromEmptyOrOpponent) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{20, 18}, {9, 6}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, WrongDirectionWhite) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{10, 12}, {10, 13}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, WrongDirectionRed) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // Red move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{5, 3}, {5, 2}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, WrongMove_LowerInsteadOfHigher) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 1;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{10, 8}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 14}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, MoveFromEmptyBar) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{10, 8}, {FROM_BAR, 21}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, InvalidPosition1) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[9][RED] = 2;
  board[5][RED] = 2;
  board[1][WHITE] = 3;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[1]);

  // White move
  setDice_forTest({5, 6});
  ASSERT_FALSE(update(make_command({{1, -4}, {1, -5}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[1]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, InvalidPosition2) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[23][RED] = 2;
  board[10][WHITE] = 3;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[23]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);

  // Red move
  setDice_forTest({6, 5});
  ASSERT_FALSE(update(make_command({{23, 29}, {23, 28}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[23]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{13, 12}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, InvalidBearingOff_NotAllHome) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 3;
  board[9][RED] = 2;
  board[5][RED] = 2;
  board[2][WHITE] = 1;
  board[1][WHITE] = 1;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{2, BEAR_OFF}, {1, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 10}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, InvalidBearingOff_OneInBar) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[10][WHITE] = 1;
  board[9][RED] = 2;
  board[5][RED] = 2;
  board[2][WHITE] = 1;
  board[1][WHITE] = 1;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[10]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);

  // Red move
  setDice_forTest({1, 3});
  ASSERT_TRUE(update(make_command({{9, 10}, {9, 12}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 1}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // White move
  setDice_forTest({2, 3});
  ASSERT_FALSE(update(make_command({{2, BEAR_OFF}, {1, BEAR_OFF}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[12]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[10]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 1}), game_wm.bar);
  ASSERT_EQ((Point{11, 12}), game_wm.borne_off);

  ASSERT_TRUE(isFinished());
}

TEST_F(GameLogic_ExpectedFalse_Test, Invalid_BearingOffLowerInsteadOfHigher) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[9][RED] = 2;
  board[5][RED] = 2;
  board[4][WHITE] = 2;
  board[1][WHITE] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[1]);

  // White move
  setDice_forTest({3, 4});
  ASSERT_FALSE(update(make_command({{1, BEAR_OFF}, {1, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[1]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 11}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, Invalid_NotBearingOffHighest) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[9][RED] = 2;
  board[5][RED] = 2;
  board[2][WHITE] = 2;
  board[1][WHITE] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[1]);

  // White move
  setDice_forTest({5, 4});
  ASSERT_FALSE(update(make_command({{1, BEAR_OFF}, {1, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{2, 0}), game_wm.board[9]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[2]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[1]);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{11, 11}), game_wm.borne_off);
}

TEST_F(GameLogic_ExpectedFalse_Test, BearingOff_OnlyOnePiece) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[20][RED] = 1;
  board[4][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{14, 14}), game_wm.borne_off);

  // White move
  setDice_forTest({5, 2});
  ASSERT_FALSE(update(make_command({{4, BEAR_OFF}}), 9091));
  ASSERT_TRUE(isFinished());
}

TEST_F(GameLogic_ExpectedFalse_Test, CannotBearOff_MustUseTwoDice) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[20][RED] = 1;
  board[5][WHITE] = 1;
  board[4][RED] = 2;
  board[1][WHITE] = 1;
  setBoard_forTest(board);

  // Initial State
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[5]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{12, 13}), game_wm.borne_off);

  // White move
  setDice_forTest({1, 2});
  ASSERT_FALSE(update(make_command({{1, BEAR_OFF}}), 9091));
  ASSERT_TRUE(isFinished());
}

TEST_F(GameLogic_ExpectedFalse_Test, NotMandatoryMove) {
  // Setup
  vector<Point> board(EMPTY_BOARD);
  board[4][WHITE] = 1;
  board[3][WHITE] = 1;
  board[1][RED] = 2;
  setBoard_forTest(board);

  // Initial state
  auto game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[3]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[1]);
  ASSERT_EQ((Point{13, 13}), game_wm.borne_off);

  // White move
  setDice_forTest({5, 2});
  ASSERT_FALSE(update(make_command({{4, BEAR_OFF}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 1}), game_wm.board[4]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[3]);
  ASSERT_EQ((Point{2, 0}), game_wm.board[1]);
  ASSERT_EQ((Point{13, 13}), game_wm.borne_off);

  ASSERT_TRUE(isFinished());

  // Final state
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
}

TEST_F(GameLogic_Functions_Test, ColorTest) {
  ASSERT_EQ(PlayerColor::RED, color_(9090));
  ASSERT_EQ(PlayerColor::WHITE, color_(9091));
}

TEST_F(GameLogic_Functions_Test, AllCheckersInRedHomeTest) {
  // Empty
  vector<Point> board(EMPTY_BOARD);
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, RED));

  // Only one
  board[20][RED] = 1;
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, RED));

  // Limits
  board[23][RED] = 2;
  board[18][RED] = 3;
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, RED));

  // Other player
  board[10][WHITE] = 1;
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, RED));

  // Not in home
  board[11][RED] = 1;
  setBoard_forTest(board);
  ASSERT_FALSE(all_checkers_in_home_board_(worldModel_, RED));

  // In other home
  board = EMPTY_BOARD;
  board[0][RED] = 1;
  setBoard_forTest(board);
  ASSERT_FALSE(all_checkers_in_home_board_(worldModel_, RED));
}

TEST_F(GameLogic_Functions_Test, AllCheckersInWhiteHomeTest) {
  // Empty
  vector<Point> board(EMPTY_BOARD);
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, WHITE));

  // Only one
  board[2][WHITE] = 1;
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, WHITE));

  // Limits
  board[5][WHITE] = 2;
  board[0][WHITE] = 3;
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, WHITE));

  // Other player
  board[4][RED] = 1;
  setBoard_forTest(board);
  ASSERT_TRUE(all_checkers_in_home_board_(worldModel_, WHITE));

  // Not in home
  board[11][WHITE] = 1;
  setBoard_forTest(board);
  ASSERT_FALSE(all_checkers_in_home_board_(worldModel_, WHITE));

  // In other home
  board = EMPTY_BOARD;
  board[20][WHITE] = 1;
  setBoard_forTest(board);
  ASSERT_FALSE(all_checkers_in_home_board_(worldModel_, WHITE));
}

TEST_F(GameLogic_Functions_Test, AllCheckersInHome_ButBarTest) {
  // Empty
  vector<Point> board(EMPTY_BOARD);
  setBoard_forTest(board);
  worldModel_.bar = {1, 0};
  ASSERT_FALSE(all_checkers_in_home_board_(worldModel_, RED));
}

TEST_F(GameLogic_Functions_Test, TryMoveTest_Simple) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[10][WHITE] = 2;

  // Simple move
  WorldModel new_wm;
  ASSERT_TRUE(try_move_(wm, 10, 9, WHITE, new_wm));
  ASSERT_EQ(1, new_wm.board[10][WHITE]);
  ASSERT_EQ(1, new_wm.board[9][WHITE]);

  wm = new_wm;
  ASSERT_TRUE(try_move_(wm, 9, 8, WHITE, new_wm));
  ASSERT_EQ(1, new_wm.board[10][WHITE]);
  ASSERT_EQ(0, new_wm.board[9][WHITE]);
  ASSERT_EQ(1, new_wm.board[8][WHITE]);

  // Falsy
  ASSERT_FALSE(try_move_(wm, 20, 19, WHITE, new_wm));
  ASSERT_FALSE(try_move_(wm, 25, 20, WHITE, new_wm));
  ASSERT_FALSE(try_move_(wm, 2, -3, WHITE, new_wm));
}

TEST_F(GameLogic_Functions_Test, TryMoveTest_Hit) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[10][WHITE] = 1;
  wm.board[9][RED] = 2;
  wm.board[8][RED] = 1;
  wm.bar = {0, 0};

  // Blocked move
  WorldModel new_wm;
  ASSERT_FALSE(try_move_(wm, 10, 9, WHITE, new_wm));

  // Hit
  ASSERT_TRUE(try_move_(wm, 10, 8, WHITE, new_wm));
  ASSERT_EQ(0, new_wm.board[10][WHITE]);
  ASSERT_EQ(2, new_wm.board[9][RED]);
  ASSERT_EQ(1, new_wm.board[8][WHITE]);
  ASSERT_EQ(1, new_wm.bar[RED]);
}

TEST_F(GameLogic_Functions_Test, TryMoveTest_FromBar) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[2][WHITE] = 2;
  wm.bar = {1, 0};

  // Blocked move
  WorldModel new_wm;
  ASSERT_FALSE(try_move_(wm, FROM_BAR, 2, RED, new_wm));

  // Hit
  ASSERT_TRUE(try_move_(wm, FROM_BAR, 1, RED, new_wm));
  ASSERT_EQ(2, new_wm.board[2][WHITE]);
  ASSERT_EQ(1, new_wm.board[1][RED]);
  ASSERT_EQ(0, new_wm.bar[RED]);
}

TEST_F(GameLogic_Functions_Test, TryMoveTest_TwoInBar) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[2][WHITE] = 2;
  wm.bar = {2, 0};

  // Blocked move
  WorldModel new_wm;
  ASSERT_FALSE(try_move_(wm, FROM_BAR, 2, RED, new_wm));

  // Hit
  ASSERT_TRUE(try_move_(wm, FROM_BAR, 1, RED, new_wm));
  ASSERT_EQ(2, new_wm.board[2][WHITE]);
  ASSERT_EQ(1, new_wm.board[1][RED]);
  ASSERT_EQ(1, new_wm.bar[RED]);
}

TEST_F(GameLogic_Functions_Test, TryMoveTest_BearOff) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[2][WHITE] = 2;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};

  // Bear off
  WorldModel new_wm;
  ASSERT_TRUE(try_move_(wm, 2, BEAR_OFF, WHITE, new_wm));
  ASSERT_EQ(1, new_wm.board[2][WHITE]);
  ASSERT_EQ(1, new_wm.borne_off[WHITE]);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_Empty) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {1, 1};

  std::vector<Command> possible_commands = calculate_possibilities_(wm, Command(), WHITE);

  ASSERT_EQ(static_cast<size_t>(1), possible_commands.size());
  ASSERT_EQ(static_cast<size_t>(0), possible_commands[0].moves.size());
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_OneChecker) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[6][WHITE] = 1;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = { make_command({{6, 5}, {5, 3}}), make_command({{6, 5}}), make_command({}) };

  /*std::cout << "[";
  for (const auto& cmd : possible_commands_vector) {
    std::cout << cmd << ", ";
  }
  std::cout << "]" << std::endl;*/

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_TwoCheckersSamePoint) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[6][WHITE] = 2;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = { make_command({{6, 5}, {5, 3}}), make_command({{6, 5}, {6, 4}}), make_command({{6, 5}}), make_command({}) };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_TwoCheckersDifferentPoints) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[16][WHITE] = 1;
  wm.board[6][WHITE] = 1;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{16, 15}}),
        make_command({{16, 15}, {15, 13}}),
        make_command({{16, 15}, {6, 4}}),
      make_command({{6, 5}}),
        make_command({{6, 5}, {16, 14}}),
        make_command({{6, 5}, {5, 3}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_OneCheckerBlocked) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[16][WHITE] = 1;
  wm.board[15][RED] = 2;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({})
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_OneCheckerHit) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[16][WHITE] = 1;
  wm.board[15][RED] = 1;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{16, 15}}),
        make_command({{16, 15}, {15, 13}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_FromBar_NothingInBoard) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.bar = {0, 1};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{FROM_BAR, 23}}),
        make_command({{FROM_BAR, 23}, {23, 21}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_FromBar_SomethingInBoard) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[16][WHITE] = 1;
  wm.bar = {0, 1};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{FROM_BAR, 23}}),
        make_command({{FROM_BAR, 23}, {23, 21}}),
        make_command({{FROM_BAR, 23}, {16, 14}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_TwoInBar_OneInBoard) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[16][WHITE] = 1;
  wm.bar = {0, 2};
  wm.borne_off = {0, 0};
  wm.dice = {1, 2};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{FROM_BAR, 23}}),
        make_command({{FROM_BAR, 23}, {FROM_BAR, 22}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_BearingOff) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[4][WHITE] = 1;
  wm.board[2][WHITE] = 1;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {5, 3};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{4, BEAR_OFF}}),
        make_command({{4, BEAR_OFF}, {2, BEAR_OFF}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_BearingOff_MustOnlyHigher) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[5][WHITE] = 2;
  wm.board[0][WHITE] = 1;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {2, 3};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{5, 3}}),
        make_command({{5, 3}, {5, 2}}),
        make_command({{5, 3}, {3, 0}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_BearingOff_NoHigherCanExactAndLower) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[1][WHITE] = 1;
  wm.board[0][WHITE] = 2;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {2, 3};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{1, BEAR_OFF}}),
        make_command({{1, BEAR_OFF}, {0, BEAR_OFF}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_BearingOff_NoHigherCanOnlyLower) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[1][WHITE] = 1;
  wm.board[0][WHITE] = 2;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {3, 4};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{1, BEAR_OFF}}),
        make_command({{1, BEAR_OFF}, {0, BEAR_OFF}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_BearingOff_MustHigherThenCanLower) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[5][WHITE] = 1;
  wm.board[0][WHITE] = 1;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {4, 3};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{5, 1}}),
        make_command({{5, 1}, {1, BEAR_OFF}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_BearingOff_OnlyOneInBoard) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[5][WHITE] = 1;
  wm.bar = {0, 0};
  wm.borne_off = {0, 0};
  wm.dice = {1, 6};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), WHITE);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{5, 4}}),
        make_command({{5, 4}, {4, BEAR_OFF}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, PossibleCommandsTest_BlockedBar) {
  // Setup
  WorldModel wm;
  wm.board = EMPTY_BOARD;
  wm.board[5][WHITE] = 5;
  wm.board[3][WHITE] = 1;
  wm.bar = {2, 0};
  wm.borne_off = {0, 0};
  wm.dice = {3, 6};

  std::vector<Command> possible_commands_vector = calculate_possibilities_(wm, Command(), RED);
  std::unordered_set<Command> possible_commands_set(possible_commands_vector.begin(), possible_commands_vector.end());
  std::unordered_set<Command> expected = {
    make_command({}),
      make_command({{FROM_BAR, 2}}),
  };

  ASSERT_EQ(expected, possible_commands_set);
}

TEST_F(GameLogic_Functions_Test, FilterCommandsTest_NumberOfMoves) {
  worldModel_.dice = {1, 4};
  std::vector<Command> commands = filter_commands_({ make_command({{1, 2}}), make_command({}), make_command({{3, 4}, {20, BEAR_OFF}}), make_command({{5, 9}}), make_command({{FROM_BAR, 3}, {5, 6}}),}, RED);
  std::unordered_set<Command> filtered_commands(commands.begin(), commands.end());

  std::unordered_set<Command> expected_commands = { make_command({{3, 4}, {20, BEAR_OFF}}), make_command({{FROM_BAR, 3}, {5, 6}}),};

  ASSERT_EQ(expected_commands, filtered_commands);
}

TEST_F(GameLogic_Functions_Test, FilterCommandsTest_HighestDie) {
  worldModel_.dice = {1, 2};
  std::vector<Command> commands = filter_commands_({ make_command({{1, 2}}), make_command({}), make_command({{4, 6}}), make_command({{FROM_BAR, 1}}),}, RED);
  std::unordered_set<Command> filtered_commands(commands.begin(), commands.end());

  std::unordered_set<Command> expected_commands = { make_command({{4, 6}}), make_command({{FROM_BAR, 1}}),};

  ASSERT_EQ(expected_commands, filtered_commands);
}

TEST_F(GameLogic_Functions_Test, FilterCommandsTest_BlockedBar) {
  worldModel_.dice = {3, 6};
  std::vector<Command> commands = filter_commands_({ make_command({}), make_command({{FROM_BAR, 2}}), }, RED);
  std::unordered_set<Command> filtered_commands(commands.begin(), commands.end());

  std::unordered_set<Command> expected_commands = { make_command({{FROM_BAR, 2}}), };

  ASSERT_EQ(expected_commands, filtered_commands);
}

TEST_F(GameLogic_IntegrationAndStress_Test, LongTest) {
  // No setup, since it was made in the constructor

  // Red move
  setDice_forTest({1, 3});
  ASSERT_TRUE(update(make_command({{0, 1}, {0, 3}}), 9090));

  WorldModel game_wm = getWorldModel();
  ASSERT_EQ((Point{1, 0}), game_wm.board[3]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{0, 0}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // White move
  setDice_forTest({2, 4});
  ASSERT_TRUE(update(make_command({{5, 3}, {5, 1}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 3}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[3]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{2, 0}), game_wm.bar);
  ASSERT_EQ((Point{0, 0}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // Red move
  setDice_forTest({2, 4});
  ASSERT_FALSE(update(make_command({{18, 20}, {18, 22}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 0}), game_wm.board[22]);
  ASSERT_EQ((Point{0, 0}), game_wm.board[20]);
  ASSERT_EQ((Point{5, 0}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[3]);
  ASSERT_EQ((Point{0, 1}), game_wm.board[1]);
  ASSERT_EQ((Point{2, 0}), game_wm.bar);
  ASSERT_EQ((Point{0, 0}), game_wm.borne_off);

  ASSERT_TRUE(isFinished());
  hasFinished_ = false;

  // Red move
  setDice_forTest({2, 4});
  ASSERT_TRUE(update(make_command({{FROM_BAR, 1}, {FROM_BAR, 3}}), 9090));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{5, 0}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[5]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[3]);
  ASSERT_EQ((Point{1, 0}), game_wm.board[1]);
  ASSERT_EQ((Point{0, 2}), game_wm.bar);
  ASSERT_EQ((Point{0, 0}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());

  // White move
  setDice_forTest({6, 6});
  ASSERT_FALSE(update(make_command({{FROM_BAR, 18}, {FROM_BAR, 18}, {18, 12}, {18, 12}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{5, 0}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 5}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 2}), game_wm.bar);
  ASSERT_EQ((Point{0, 0}), game_wm.borne_off);

  ASSERT_TRUE(isFinished());
  hasFinished_ = false;

  // White move
  setDice_forTest({5, 5});
  ASSERT_TRUE(update(make_command({{FROM_BAR, 19}, {FROM_BAR, 19}, {19, 14}, {19, 14}}), 9091));

  game_wm = getWorldModel();
  ASSERT_EQ((Point{0, 0}), game_wm.board[19]);
  ASSERT_EQ((Point{5, 0}), game_wm.board[18]);
  ASSERT_EQ((Point{0, 2}), game_wm.board[14]);
  ASSERT_EQ((Point{0, 5}), game_wm.board[12]);
  ASSERT_EQ((Point{0, 3}), game_wm.board[5]);
  ASSERT_EQ((Point{0, 0}), game_wm.bar);
  ASSERT_EQ((Point{0, 0}), game_wm.borne_off);

  ASSERT_FALSE(isFinished());
}

}}  // namespaces

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
