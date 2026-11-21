#include "length_monitor/stats.hpp"
#include <numeric>

std::optional<double> meanLength(const std::vector<double>& lengths) {
  if (lengths.empty()) {
    return std::nullopt;
  }
  double sum = std::accumulate(lengths.begin(), lengths.end(), 0.0);
  return sum / static_cast<double>(lengths.size());
}

std::size_t countAtLeastThreshold(const std::vector<double>& lengths, double threshold) {
  std::size_t count = 0;
  for (double val : lengths) {
    if (val >= threshold) {
      ++count;
    }
  }
  return count;
}
