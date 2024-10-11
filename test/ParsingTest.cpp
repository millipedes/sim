#include <gtest/gtest.h>

#include "Parsing.h"

TEST(parsing, json_parse_test_0) {
  auto expected_commands = parse_json(R"({
  "d": {
    "address": 1
  },
  "a": {
    "address": 1,
    "arguments": ["my_text", "my_other_text"]
  },
  "s": {
    "arguments": ["a+", "b", "g"]
  }
})");
  ASSERT_TRUE(expected_commands);
  ASSERT_EQ(Command("d", std::nullopt, 1), expected_commands.value()[0]);
  ASSERT_EQ(Command("a", Strings{"my_text", "my_other_text"}, 1),
      expected_commands.value()[1]);
  ASSERT_EQ(Command("s", Strings{"a+", "b", "g"}, std::nullopt),
      expected_commands.value()[2]);
}
