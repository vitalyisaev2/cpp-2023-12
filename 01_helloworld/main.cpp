#include <iostream>
#include <sstream>

#include "version.h"

int main() {
  std::stringstream ss;
  ss << "build " << HelloWorld_VERSION_MAJOR << "." << HelloWorld_VERSION_MINOR
     << "." << HelloWorld_VERSION_PATCH << std::endl;
  ss << "Hello, World!" << std::endl;

  std::cout << ss.str();

  return 0;
}
