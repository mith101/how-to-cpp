// Copyright (c) 2024 Michael Thoma
// This program and the accompanying materials are made available under the
// terms of the MIT License, which is available in the project root.
#include "dbscan.hpp"

#include <functional>
#include <list>
#include <vector>

std::string ToString(Type type) {
  switch (type) {
    case Type::kNotVisited:
      return "NotVisited";
    case Type::kCore:
      return "Core";
    case Type::kReachable:
      return "Reachable";
    case Type::kNoise:
      return "Noise";
  }
  return "[ERROR]";
};

std::vector<size_t> RegionQuery(const std::vector<Point>& points,
                                const Point& point_under_test, double eps) {
  std::vector<size_t> neighbours;
  size_t index = 0;
  for (const auto& point : points) {
    if (Distance(point_under_test, point) < eps) {
      neighbours.emplace_back(index);
    }
    ++index;
  }
  return neighbours;
}

void ExpandCluster(const std::vector<Point>& points, size_t index_under_test,
                   size_t current_cluster,
                   const std::vector<size_t>& neighbours,
                   std::vector<DbscanResult>& result, double eps,
                   size_t min_points) {
  result.at(index_under_test).cluster = current_cluster;
  std::list<std::vector<size_t>> additional_neighbours;
  std::list<std::reference_wrapper<const std::vector<size_t>>> all_neighbours;
  all_neighbours.emplace_back(std::cref(neighbours));
  for (const auto& neighbours : all_neighbours) {
    for (auto index_prime : neighbours.get()) {
      if (result.at(index_prime).type == Type::kNotVisited) {
        result.at(index_prime).type = Type::kReachable;
        const auto neighbours_prime =
            RegionQuery(points, points.at(index_prime), eps);
        if (min_points <= neighbours_prime.size()) {
          result.at(index_prime).type = Type::kCore;
          additional_neighbours.emplace_back(std::move(neighbours_prime));
          all_neighbours.emplace_back(std::cref(additional_neighbours.back()));
        }
      }
      if (result.at(index_prime).cluster == kNoCluster) {
        result.at(index_prime).cluster = current_cluster;
      }
    }
  }
}

std::vector<DbscanResult> Dbscan(const std::vector<Point>& points, double eps,
                                 size_t min_points) {
  std::vector<DbscanResult> result(points.size());
  size_t current_cluster = kNoCluster;
  size_t index = 0;
  for (const auto& point : points) {
    if (result.at(index).type == Type::kNotVisited) {
      result.at(index).type = Type::kNoise;
      const auto neighbours = RegionQuery(points, point, eps);
      if (min_points <= neighbours.size()) {
        result.at(index).type = Type::kCore;
        ++current_cluster;
        ExpandCluster(points, index, current_cluster, neighbours, result, eps,
                      min_points);
      }
    }
    ++index;
  }
  return result;
}
