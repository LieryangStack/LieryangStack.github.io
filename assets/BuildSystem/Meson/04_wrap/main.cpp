#include <stdio.h>
#include <gtest/gtest.h>
#include "sum.h"

TEST(test_case_name, test_name) {
  EXPECT_EQ(2, sum(1, 1));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}