#include <fstream>
#include <gtest/gtest.h>

#include "Context.h"
#include "Version.h"

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

TEST(execution, branch_test_0) {
  auto result = execute(line_one_through_five, R"({
  "b": {
    "arguments": ["test"]
  },
  "d": { },
  ":": {
    "arguments": ["test"]
  },
  "p": { }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #2
This is line #3
This is line #3
This is line #4
This is line #4
This is line #5
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_test_1) {
  auto result = execute(line_one_through_five, R"({
  "b": {
    "arguments": ["test"]
  },
  "p": { },
  ":": {
    "arguments": ["test"]
  },
  "d": { }
})");

  auto expected_output = R"()";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_test_2) {
  auto result = execute(line_one_through_five, R"({
  "b": {
    "address": 2,
    "arguments": ["test"]
  },
  "d": { },
  ":": {
    "arguments": ["test"]
  },
  "p": { }
})");

  auto expected_output = R"(This is line #2
This is line #2
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
  auto result = execute(line_one_through_five, R"({ "d": {} })");

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

TEST(execution, delete_restart_test_0) {
  auto result = execute(line_one_through_five, R"({
  "N": {
    "address": 1
  },
  "D": {
    "address": 2
  }
})");

  auto expected_output = R"(This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, delete_restart_test_1) {
  auto result = execute(line_one_through_five, R"({
  "N": { },
  "D": { }
})");

  auto expected_output = R"(This is line #5
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
  testing::internal::CaptureStderr();
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
  testing::internal::GetCapturedStderr();

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

TEST(execution, prepend_file_name_0) {
  auto result = execute(line_one_through_five, R"({
  "F": {}
})", "./test.txt");

  auto expected_output = R"(./test.txt
This is line #1
./test.txt
This is line #2
./test.txt
This is line #3
./test.txt
This is line #4
./test.txt
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, prepend_file_name_1) {
  auto result = execute(line_one_through_five, R"({
  "F": {
    "address": 3
  }
})", "./test.txt");

  auto expected_output = R"(This is line #1
This is line #2
./test.txt
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
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

TEST(execution, next_operation_space_0) {
  auto result = execute(line_one_through_five, R"({
  "n": { },
  "p": { }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #2
This is line #3
This is line #4
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, next_operation_space_1) {
  auto result = execute(line_one_through_five, R"({
  "n": { },
  "d": { }
})");

  auto expected_output = R"(This is line #1
This is line #3
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, next_operation_space_2) {
  auto result = execute(line_one_through_five, R"({
  "n": {
    "address": 2
  },
  "d": { }
})");

  auto expected_output = R"(This is line #2
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, append_next_operation_space_0) {
  auto result = execute(line_one_through_five, R"({
  "N": { },
  "s": {
    "arguments": ["\\n", " "]
  }
})");

  auto expected_output = R"(This is line #1 This is line #2
This is line #3 This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, append_next_operation_space_1) {
  auto result = execute(line_one_through_five, R"({
  "N": { },
  "s": {
    "address": 3,
    "arguments": ["\\n", " "]
  }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, append_next_operation_space_2) {
  auto result = execute(line_one_through_five, R"({
  "N": {
    "address": 3
  },
  "s": {
    "arguments": ["\\n", " "]
  }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #3 This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, print_test_0) {
  auto result = execute(line_one_through_five, R"({
  "p": { }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #2
This is line #3
This is line #3
This is line #4
This is line #4
This is line #5
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, print_test_1) {
  auto result = execute(line_one_through_five, R"({
  "p": {
    "address": 1
  }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, nl_print_test_0) {
  auto result = execute(line_one_through_five, R"({
  "P": { }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #2
This is line #3
This is line #3
This is line #4
This is line #4
This is line #5
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, nl_print_test_1) {
  auto result = execute(line_one_through_five, R"({
  "P": {
    "address": 1
  }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, read_in_file_test_0) {
  auto result = execute(line_one_through_five, R"({
    "r": {
      "arguments": ["../resources/read_in_file_test.txt"]
    }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
This is line #2
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
This is line #3
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
This is line #4
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
This is line #5
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, read_in_file_test_1) {
  auto result = execute(line_one_through_five, R"({
    "r": {
      "address": 3,
      "arguments": ["../resources/read_in_file_test.txt"]
    }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #3
This is line #1
This is line #2
This is line #3
This is line #4
This is line #5
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, read_in_file_line_test_0) {
  auto result = execute(line_one_through_five, R"({
    "R": {
      "arguments": ["../resources/read_in_file_line_eq.txt"]
    }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #2
This is line #3
This is line #3
This is line #4
This is line #4
This is line #5
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, read_in_file_line_test_1) {
  auto result = execute(line_one_through_five, R"({
    "R": {
      "arguments": ["../resources/read_in_file_line_gt.txt"]
    }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #2
This is line #3
This is line #3
This is line #4
This is line #4
This is line #5
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, read_in_file_line_test_2) {
  auto result = execute(line_one_through_five, R"({
    "R": {
      "arguments": ["../resources/read_in_file_line_lt.txt"]
    }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #2
This is line #2
This is line #3
This is line #3
This is line #4
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, read_in_file_line_test_3) {
  auto result = execute(line_one_through_five, R"({
    "R": {
      "address": 3,
      "arguments": ["../resources/read_in_file_line_eq.txt"]
    }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #3
This is line #1
This is line #4
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, substitute_test_0) {
  auto result = execute(R"(The fox is quick
The dog is quick
)", R"({
  "s": {
    "arguments": ["quick", "swift"]
  }
})");

  auto expected_output = R"(The fox is swift
The dog is swift
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, substitute_test_1) {
  auto result = execute(R"(The date is 12/01/2023
The date will be 01/15/2024
)", R"({
  "s": {
    "arguments": ["()" R"(\\d{2})/(\\d{2})/(\\d{4}))" R"(", "$3-$1-$2"]
  }
})");

  auto expected_output = R"(The date is 2023-12-01
The date will be 2024-01-15
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, substitute_test_2) {
  auto result = execute(R"(The date is 12/01/2023
The date will be 01/15/2024
)", R"({
  "s": {
    "address": 1,
    "arguments": ["()" R"(\\d{2})/(\\d{2})/(\\d{4}))" R"(", "$3-$1-$2"]
  }
})");

  auto expected_output = R"(The date is 2023-12-01
The date will be 01/15/2024
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_true_test_0) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["world", "universe"]
  },
  "t": {
    "arguments": ["test"]
  },
  "p": { },
  ":": {
    "arguments": ["test"]
  },
  "d": { }
})");

  auto expected_output = "";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_true_test_1) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["world", "universe"]
  },
  "t": {
    "arguments": ["test"]
  },
  "d": { },
  ":": {
    "arguments": ["test"]
  },
  "p": { }
})");

  auto expected_output = R"(Hello universe
Hello universe
This is a message to the universe
This is a message to the universe
That sim is complete for the universe to use!
That sim is complete for the universe to use!
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_true_test_2) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["world", "universe"]
  },
  "t": {
    "address": 2,
    "arguments": ["test"]
  },
  "d": { },
  ":": {
    "arguments": ["test"]
  },
  "p": { }
})");

  auto expected_output = R"(This is a message to the universe
This is a message to the universe
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_true_test_3) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["world", "universe"]
  },
  "t": {
    "arguments": ["test"]
  },
  "d": { },
  "p": { }
})");

  auto expected_output = R"(Hello universe
This is a message to the universe
That sim is complete for the universe to use!
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_false_test_0) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["World", "universe"]
  },
  "T": {
    "arguments": ["test"]
  },
  "p": { },
  ":": {
    "arguments": ["test"]
  },
  "d": { }
})");

  auto expected_output = "";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_false_test_1) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["World", "universe"]
  },
  "T": {
    "arguments": ["test"]
  },
  "d": { },
  ":": {
    "arguments": ["test"]
  },
  "p": { }
})");

  auto expected_output = R"(Hello world
Hello world
This is a message to the world
This is a message to the world
That sim is complete for the world to use!
That sim is complete for the world to use!
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_false_test_2) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["World", "universe"]
  },
  "T": {
    "address": 2,
    "arguments": ["test"]
  },
  "d": { },
  ":": {
    "arguments": ["test"]
  },
  "p": { }
})");

  auto expected_output = R"(This is a message to the world
This is a message to the world
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, branch_false_test_3) {
  auto result = execute(R"(Hello world
This is a message to the world
That sim is complete for the world to use!
)", R"({
  "s": {
    "arguments": ["world", "universe"]
  },
  "T": {
    "arguments": ["test"]
  },
  "d": { },
  "p": { }
})");

  auto expected_output = R"(Hello universe
This is a message to the universe
That sim is complete for the universe to use!
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, version_test_0) {
  try {
    auto result = execute(line_one_through_five, R"({
    "v": {
      "arguments": ["9.9.9"]
    }
})");
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ((std::string("execute: unable to execute command: "
            "assert_version_function: version required: 9.9.9 does not match "
            "current version: ") + std::string(sim_version)).c_str(), e.what());
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}

TEST(execution, append_to_file_test_0) {
  // We want to be able to run this over and over without having to remove the
  // file, this clears it each run
  std::ofstream clean_out_file("append_to_file_test_0.txt", std::ios::trunc);
  clean_out_file.close();

  auto result = execute(line_one_through_five, R"({
  "w": {
    "arguments": ["append_to_file_test_0.txt"]
  }
})");

  auto expected_output = file_to_string("append_to_file_test_0.txt");
  ASSERT_TRUE(expected_output);

  ASSERT_EQ(result, expected_output.value());
}

TEST(execution, append_to_file_test_1) {
  // We want to be able to run this over and over without having to remove the
  // file, this clears it each run
  std::ofstream clean_out_file("append_to_file_test_1.txt", std::ios::trunc);
  clean_out_file.close();

  auto result = execute(line_one_through_five, R"({
  "w": {
    "address": 3,
    "arguments": ["append_to_file_test_1.txt"]
  }
})");

  auto expected_output = file_to_string("append_to_file_test_1.txt");
  ASSERT_TRUE(expected_output);
  ASSERT_EQ(R"(This is line #3
)", expected_output.value());

  ASSERT_EQ(result, line_one_through_five);
}

TEST(execution, nl_append_to_file_test_0) {
  // We want to be able to run this over and over without having to remove the
  // file, this clears it each run
  std::ofstream clean_out_file("nl_append_to_file_test_0.txt", std::ios::trunc);
  clean_out_file.close();

  auto result = execute(line_one_through_five, R"({
  "W": {
    "arguments": ["nl_append_to_file_test_0.txt"]
  }
})");

  auto expected_output = file_to_string("nl_append_to_file_test_0.txt");
  ASSERT_TRUE(expected_output);

  ASSERT_EQ(result, expected_output.value());
}

TEST(execution, nl_append_to_file_test_1) {
  // We want to be able to run this over and over without having to remove the
  // file, this clears it each run
  std::ofstream clean_out_file("nl_append_to_file_test_1.txt", std::ios::trunc);
  clean_out_file.close();

  auto result = execute(line_one_through_five, R"({
  "W": {
    "address": 3,
    "arguments": ["nl_append_to_file_test_1.txt"]
  }
})");

  auto expected_output = file_to_string("nl_append_to_file_test_1.txt");
  ASSERT_TRUE(expected_output);
  ASSERT_EQ(R"(This is line #3
)", expected_output.value());

  ASSERT_EQ(result, line_one_through_five);
}

TEST(execution, exchange_test_0) {
  auto result = execute(line_one_through_five, R"({
  "h": {
    "address": 1
  },
  "x": {
    "address": 3
  },
  "g": { }
})");

  auto expected_output = R"(This is line #1
This is line #1
This is line #3
This is line #3
This is line #3
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, exchange_test_1) {
  auto result = execute(line_one_through_five, R"({
  "h": {
    "address": 1
  },
  "x": {
    "address": 3
  },
  "g": {
    "address": 4
  }
})");

  auto expected_output = R"(This is line #1
This is line #2
This is line #1
This is line #3
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

TEST(execution, zap_test_0) {
  auto result = execute(line_one_through_five, R"({
  "z": { }
})");

  auto expected_output = R"(




)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, zap_test_1) {
  auto result = execute(line_one_through_five, R"({
  "z": {
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

TEST(execution, prepend_line_no_test_0) {
  auto result = execute(line_one_through_five, R"({
  "=": { }
})");

  auto expected_output = R"(1
This is line #1
2
This is line #2
3
This is line #3
4
This is line #4
5
This is line #5
)";

  ASSERT_EQ(result, expected_output);
}

TEST(execution, prepend_line_no_test_1) {
  auto result = execute(line_one_through_five, R"({
  "=": {
    "address": 2
  }
})");

  auto expected_output = R"(This is line #1
2
This is line #2
This is line #3
This is line #4
This is line #5
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

TEST(execution, verify_label_test_0) {
  try {
    auto result = execute(line_one_through_five, R"({ ":": { } })");
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ("execute: unable to execute command: verify_label_function: "
        "no arguments provided", e.what());
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}

TEST(execution, verify_label_test_1) {
  try {
    auto result = execute(line_one_through_five, R"({ ":": {
      "arguments": ["this should", "fail"]
    }
})");
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ("execute: unable to execute command: verify_label_function: "
        "label expects 1 argument", e.what());
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}

TEST(execution, verify_label_test_2) {
  try {
    auto result = execute(line_one_through_five, R"({ ":": {
      "arguments": ["this should fail"],
      "address": 1
    }
})");
  } catch (const std::runtime_error& e) {
    EXPECT_STREQ("execute: unable to execute command: verify_label_function: "
        "label expects no address", e.what());
  } catch (...) {
    FAIL() << "Expected std::runtime_error";
  }
}
