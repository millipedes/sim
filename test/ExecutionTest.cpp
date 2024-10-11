#include <gtest/gtest.h>

#include "Context.h"

TEST(execution, execution_test_0) {
  auto result = execute(R"(This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)", R"({
  "d": {
    "address": 3
  },
  "a": {
    "address": 2,
    "arguments": ["This is a new line"]
  }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is a new line
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}
