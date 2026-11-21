#pragma once
#include <cstddef>
#include <optional>
#include <vector>

std::optional<double> meanLength(const std::vector<double>& lengths);
std::size_t countAtLeastThreshold(const std::vector<double>& lengths, double threshold);
