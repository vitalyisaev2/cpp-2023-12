#include <iostream>
#include <sstream>

#include "version.h"

int main() {
  std::stringstream ss;
  ss << "build " << BUILD_NUMBER << std::endl;
  ss << "Hello, World!" << std::endl;

  std::cout << ss.str();

  return 0;
}
