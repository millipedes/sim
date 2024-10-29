#include "Context.h"

#include <iostream>

int main (int argc, char* argv[]) {
  if (argc != 3) {
    throw std::runtime_error("sedim requires two arguments: input, json script");
  }

  std::cout << execute_from_files(argv[1], argv[2]);
  return 0;
}
