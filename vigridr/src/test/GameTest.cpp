#include <gtest/gtest.h>

TEST(Sum, Simple) {
  EXPECT_EQ(2, 1 + 1);
} 

TEST(Sum, Hard) {
  EXPECT_EQ(17, 13 + 4);
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
} 