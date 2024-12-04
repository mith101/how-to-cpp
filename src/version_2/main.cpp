// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#include <print>
#include <vector>

#include "dbscan.hpp"
#include "point.hpp"

int main() {
  const std::vector<Point3d> points = {{0.0, 2.0, 0.0}, {1.0, 2.0, 0.0},
                                       {1.0, 3.0, 0.0}, {2.0, 4.0, 0.0},
                                       {3.0, 0.0, 0.0}, {4.0, 1.0, 0.0}};
  const double eps = 2;
  const size_t min_points = 3;

  const auto result = Dbscan(points, eps, min_points);

  size_t index = 0;
  std::print("{:>3} {:>3} {:>3} {:>7} {}\n", "x", "y", "z", "cluster", "type");
  for (auto [x, y, z] : points) {
    std::print("{:>3} {:>3} {:>3} {:>7} {}\n", x, y, z,
               result.at(index).cluster, ToString(result.at(index).type));
    ++index;
  }
}
