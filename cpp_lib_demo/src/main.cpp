#include "stats.hpp"
#include <cmath>
#include <iostream>

int main() {
  if (meanLength({}).has_value()) {
    std::cerr << "Error: empty vector mean should be std::nullopt\n";
    return 1;
  }
  if (countAtLeastThreshold({}, 10.0) != 0) {
    std::cerr << "Error: empty vector count should be 0\n";
    return 1;
  }

  std::vector<double> data = {5.0, 10.0, 15.0};
  auto avg = meanLength(data);
  if (!avg.has_value() || std::fabs(*avg - 10.0) > 1e-6) {
    std::cerr << "Error: meanLength calculation failed\n";
    return 1;
  }
  if (countAtLeastThreshold(data, 10.0) != 2) {
    std::cerr << "Error: countAtLeastThreshold boundary failed\n";
    return 1;
  }

  std::cout << "8.1 CMake tests passed!\n";
  return 0;
}
