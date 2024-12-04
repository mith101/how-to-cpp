// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#include <print>
#include <vector>

#include "dbscan.hpp"
#include "point.hpp"

int main() {
  const std::vector<Point> points = {{0.0, 2.0}, {1.0, 2.0}, {1.0, 3.0},
                                     {2.0, 4.0}, {3.0, 0.0}, {4.0, 1.0}};
  const double eps = 2;
  const size_t min_points = 3;

  const auto result = Dbscan(points, eps, min_points);

  size_t index = 0;
  std::print("{:>3} {:>3} {:>7}\n", "x", "y", "cluster");
  for (auto [x, y] : points) {
    std::print("{:>3} {:>3} {:>7}\n", x, y, result.at(index));
    ++index;
  }
}
