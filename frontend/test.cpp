#include <iostream>
#include <string>
#include <sstream>

int main() {
  std::string string("1 lopatinsadasdasdasd lopation");
  std::istringstream stream(string);
  std::string word;
  std::string login;
  std::string passw;
  stream >> word >> login >> passw;
  if (std::stoi(word) == 1) {
    std::cout << login << ' ' << passw << '\n';
  }
  return 0;
}