#include <gtest/gtest.h>

#include "Context.h"

constexpr static auto line_one_through_five = R"(This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)";

TEST(execution, append_test_0) {
  auto result = execute(line_one_through_five, R"({
  "a": {
    "arguments": ["This is a new line"]
  }
})");

  auto expected_output = R"(This is line #1
This is a new line
This is line #2
This is a new line
This is line #3
This is a new line
This is line #4
This is a new line
This is line #5
This is a new line
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, append_test_1) {
  auto result = execute(line_one_through_five, R"({
  "a": {
    "address": 2,
    "arguments": ["This is a new line"]
  }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is a new line
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, change_test_0) {
  auto result = execute(line_one_through_five, R"({
  "c": {
    "arguments": ["This is a different line"]
  }
})");

  auto expected_output = R"(This is a different line
This is a different line
This is a different line
This is a different line
This is a different line
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, change_test_1) {
  auto result = execute(line_one_through_five, R"({
  "c": {
    "address": 2,
    "arguments": ["This is a different line"]
  }
})");

  auto expected_output = R"(This is line #1
This is a different line
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, delete_test_0) {
  auto result = execute(R"(This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)", R"({ "d": {} })");

  auto expected_output = R"()";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, delete_test_1) {
  auto result = execute(line_one_through_five, R"({
  "d": {
    "address": 2
  }
})");

  auto expected_output = R"(This is line #1
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, insert_test_0) {
  auto result = execute(line_one_through_five, R"({
  "i": {
    "arguments": ["This is a new line"]
  }
})");

  auto expected_output = R"(This is a new line
This is line #1
This is a new line
This is line #2
This is a new line
This is line #3
This is a new line
This is line #4
This is a new line
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, insert_test_1) {
  auto result = execute(line_one_through_five, R"({
  "i": {
    "address": 2,
    "arguments": ["This is a new line"]
  }
})");

  auto expected_output = R"(This is line #1
This is a new line
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, execute_test_0) {
#if defined(__linux__)
  // There is a wanrning if the user just wants to execute a whole file, because
  // it would be better to put it through a proper bash interpreter
  testing::internal::CaptureStdout();
  auto result = execute(R"(echo -n "Hello World"
echo -n "Hello World"
echo -n "Hello World"
echo -n "Hello World"
)", R"({
  "e": {}
})");

  auto expected_output = R"(Hello World
Hello World
Hello World
Hello World
)";
  // Warning over
  testing::internal::GetCapturedStdout();

  ASSERT_EQ(result, expected_output);
#endif
}

TEST(execution, execute_test_1) {
#if defined(__linux__)
  auto result = execute(R"(This is line #1
echo -n "Hello World"
This is line #3
This is line #4
This is line #5
This is line #6
)", R"({
  "e": {
    "address": 2
  }
})");

  auto expected_output = R"(This is line #1
Hello World
This is line #3
This is line #4
This is line #5
This is line #6
)";

  ASSERT_EQ(result, expected_output);
#endif
}

TEST(execution, add_to_static_test_0) {
  auto result = execute(line_one_through_five, R"({
  "h": {
    "address": 2
  },
  "g": {}
})");

  auto expected_output = R"(
This is line #2
This is line #2
This is line #2
This is line #2
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, add_to_static_test_1) {
  auto result = execute(line_one_through_five, R"({
  "h": {
    "address": 2
  },
  "g": {
    "address": 3
  }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #2
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, nl_operations_add_to_static_test_0) {
  auto result = execute(line_one_through_five, R"({
  "h": {
    "address": 2
  },
  "G": { }
})");

  auto expected_output = R"(This is line #1

This is line #2
This is line #2
This is line #3
This is line #2
This is line #4
This is line #2
This is line #5
This is line #2
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, nl_operations_add_to_static_test_1) {
  auto result = execute(line_one_through_five, R"({
  "h": {
    "address": 2
  },
  "G": {
    "address": 2
  }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, nl_static_add_to_static_test_0) {
  auto result = execute(line_one_through_five, R"({
  "H": {
    "address": 2
  },
  "g": { }
})");

  auto expected_output = R"(

This is line #2

This is line #2

This is line #2

This is line #2
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, nl_static_add_to_static_test_1) {
  auto result = execute(line_one_through_five, R"({
  "H": { },
  "g": { }
})");

  auto expected_output = R"(
This is line #1

This is line #1
This is line #2

This is line #1
This is line #2
This is line #3

This is line #1
This is line #2
This is line #3
This is line #4

This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, nl_both_add_to_static_test_0) {
  auto result = execute(line_one_through_five, R"({
  "H": { },
  "G": { }
})");

  auto expected_output = R"(This is line #1

This is line #1
This is line #2

This is line #1
This is line #2
This is line #3

This is line #1
This is line #2
This is line #3
This is line #4

This is line #1
This is line #2
This is line #3
This is line #4
This is line #5

This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}


TEST(execution, translation_test_0) {
  auto result = execute(line_one_through_five, R"({
  "y": {
    "arguments": ["This", "That"]
  }
})");

  auto expected_output = R"(That is line #1
That is line #2
That is line #3
That is line #4
That is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, unamb_test_0) {
  auto result = execute(line_one_through_five, R"({"l": { } })");

  auto expected_output = R"(This is line #1$
This is line #1
This is line #2$
This is line #2
This is line #3$
This is line #3
This is line #4$
This is line #4
This is line #5$
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, unamb_test_1) {
  auto result = execute(line_one_through_five, R"({
  "l": {
    "address": 2
  }
})");

  auto expected_output = R"(This is line #1
This is line #2$
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, translation_test_1) {
  auto result = execute(line_one_through_five, R"({
  "y": {
    "arguments": ["This is", "These are"]
  }
})");

  auto expected_output = R"(These are line #1
These are line #2
These are line #3
These are line #4
These are line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, translation_test_2) {
  auto result = execute(line_one_through_five, R"({
  "y": {
    "arguments": ["This", "The"]
  }
})");

  auto expected_output = R"(The is line #1
The is line #2
The is line #3
The is line #4
The is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, combination_command_test_0) {
  auto result = execute(line_one_through_five, R"({
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
