#include <iostream>
#include <string>
#include <sstream>

int main() {
  std::string unit;
  bool we_have_the_id = false;
  std::string line;
  while (std::getline(std::cin, line)) {
    std::size_t non_empty = line.find_first_not_of(" \t\v");
    if (non_empty == std::string::npos || line[non_empty] == ';' || line[non_empty] == '|') {
      continue;
    }
    if (!we_have_the_id) {
      unit = line + " ";
      we_have_the_id = true;
      continue;
    }
    unit += line.substr(std::string{"  : values="}.size());
    std::cout << "cset " << unit << std::endl;
    we_have_the_id = false;
    unit = "";
  }
}
