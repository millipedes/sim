#include "Context.h"

#include <iostream>

int main (int argc, char* argv[]) {
  if (argc != 3) {
    throw std::runtime_error("sedim requires two arguments: input, json script");
  }

  auto maybe_input = file_to_string(argv[1]);
  if (!maybe_input) {
    throw std::runtime_error(maybe_input.error());
  }
  auto maybe_json = file_to_string(argv[2]);
  if (!maybe_json) {
    throw std::runtime_error(maybe_json.error());
  }

  std::cout << execute(maybe_input.value(), maybe_json.value());
  return 0;
}
